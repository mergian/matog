// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/runtime/intern/Context.h>
#include <matog/runtime/intern/BaseArray.h>
#include <matog/runtime/intern/exec/Profiler.h>
#include <matog/runtime/intern/Runtime.h>
#include <matog/runtime/intern/Decisions.h>
#include <matog/util/Hash.h>
#include <matog/log.h>
#include <matog/macros.h>
#include <mutex>

using namespace matog::runtime::intern::context;

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
std::map<CUcontext, Context>	Context::s_context;
std::mutex						Context::s_mutex;

//-------------------------------------------------------------------
Context& Context::get(CUcontext context) {
	LOCK(s_mutex);

	// does context exist in map?
	auto it = s_context.find(context);

	// if not: create new
	if(it == s_context.end())
		it = s_context.emplace(MAP_EMPLACE(context, context)).first;

	return it->second;
}

//-------------------------------------------------------------------
void Context::destroy(CUcontext context) {
	LOCK(s_mutex);

	auto it = s_context.find(context);
	if(it != s_context.end())
		s_context.erase(it);
}

//-------------------------------------------------------------------
Cache& Context::getCache(void) {
	return m_cache;
}
	
//-------------------------------------------------------------------
void Context::setCompilerFlags(const uint32_t kernelId, const char* flags) {
	m_compiler[kernelId] = flags;
}

//-------------------------------------------------------------------
const char*	Context::getCompilerFlags(const uint32_t kernelId) {
	auto it = m_compiler.find(kernelId);
	return it != m_compiler.end() ? it->second.c_str() : 0;
}

//-------------------------------------------------------------------
void Context::addArray(intern::BaseArray<>* ptr) {
	m_arrays.emplace(ptr);
}

//-------------------------------------------------------------------
void Context::addDecision(intern::MemArray<>* ptr, const Kernel* kernel, const GPU* gpu) {
	// remove decision if is already registered
	if(ptr->getDecisionId())
		removeDecision(ptr);

	// get decision id
	ptr->setDecisionId(Decisions::getId(ptr, kernel, gpu));

	// add to context
	ptr->setOffsetId(getNewOffsetId(ptr->getDecisionId()));
	m_decisions[getOffsetId(ptr)] = ptr;
}

//-------------------------------------------------------------------
void Context::copyDecision(intern::MemArray<>* dest, intern::MemArray<>* src) {
	// remove decision if is already registered
	if(dest->getDecisionId())
		removeDecision(dest);

	// inherit from src and assign new history id
	dest->setConfig(src->getConfig());
	dest->setEventId(src->getEventId());
	dest->setDecisionId(src->getDecisionId());
	dest->setOffsetId(getNewOffsetId(dest->getDecisionId()));

	// add to context
	m_decisions[getOffsetId(dest)] = dest;
}

//-------------------------------------------------------------------
void Context::removeDecision(intern::MemArray<>* array) {
	m_decisions.erase(getOffsetId(array));
}

//-------------------------------------------------------------------
void Context::removeArray(intern::BaseArray<>* array) {
	removeDecision((MemArray<>*)array);
	m_arrays.erase(array);
}

//-------------------------------------------------------------------
bool Context::isArray(intern::BaseArray<>* array) {
	return m_arrays.find(array) != m_arrays.end();
}

//-------------------------------------------------------------------
void Context::addAllocation(CUdeviceptr ptr, const uint64_t size) {
	m_allocations.emplace(MAP_EMPLACE(ptr, size));
}

//-------------------------------------------------------------------
void Context::removeAllocation(CUdeviceptr ptr) {
	m_allocations.erase(ptr);
}

//-------------------------------------------------------------------
uint64_t Context::getAllocationSize(CUdeviceptr ptr) {
	auto it = m_allocations.find(ptr);
	return it != m_allocations.end() ? it->second : 0;
}

//-------------------------------------------------------------------
CUcontext Context::getContext(void) {
	return m_context;
}

//-------------------------------------------------------------------
void Context::addMeta(const void* ptr, const array::field::Type type, const char* file, const int line) {
	std::ostringstream ss; ss << file << line;
	const uint32_t hash	 = util::Hash::crc32(ss.str().c_str());
	
	const uint32_t decisionId = Decisions::getId(hash, file, line);
	
	auto it = m_meta.emplace(MAP_EMPLACE(ptr, ptr, type, decisionId, file, line));
	m_metaById.emplace(MAP_EMPLACE(decisionId, it.first->second));
}

//-------------------------------------------------------------------
void Context::removeMeta(const void* ptr) {
	auto it = m_meta.find(ptr);

	if(it != m_meta.end()) {
		m_metaById.erase(it->second.getDecisionId());
		m_meta.erase(it);
	}
}

//-------------------------------------------------------------------
float Context::getMetaValue(const uint32_t decisionId, const uint32_t offset, const uint32_t dim) {
	// check if this is a user meta
	if(offset == 0) { // can only be user meta if historyOffset is 0!
		auto it = m_metaById.find(decisionId);

		if(it != m_metaById.end())
			return it->second.getValue();
	}

	// check if this is an array
	{
		const uint32_t currentOffsetId = getCurrentOffsetId(decisionId);

		if(currentOffsetId != UINT_MAX) {
			const uint32_t offsetId = currentOffsetId - offset;
			auto it = m_decisions.find(getOffsetId(decisionId, offsetId));

			if(it != m_decisions.end()) {
				assert(dim < it->second->getArray().getVDimCount());
				return (float)it->second->getCount(dim);
			}
		}
	}

	return 0;
}

//-------------------------------------------------------------------
uint32_t Context::getCurrentOffsetId(const uint32_t decisionId) const {
	const auto it = m_decisionOffsets.find(decisionId);
	
	if(it == m_decisionOffsets.end())
		return UINT_MAX;

	return it->second-1;
}

//-------------------------------------------------------------------
uint32_t Context::getNewOffsetId(const uint32_t decisionId) {
	return m_decisionOffsets[decisionId]++;
}

//-------------------------------------------------------------------
void Context::dbInsertCallMeta(const uint32_t eventId, DeviceArray* array) {
	const uint32_t dims = array->getArray().getVDimCount();
	const uint32_t offset = getCurrentOffsetId(array->getDecisionId()) - array->getOffsetId();

	if(dims == 0) {
		event::dbInsertEventData(eventId, array->getEventId(), array->getParamId(), array->getDecisionId(), offset, 0, 0);
	} else {
		for(uint32_t dim = 0; dim < dims; dim++)
			event::dbInsertEventData(eventId, array->getEventId(), array->getParamId(), array->getDecisionId(), offset, dim, (float)array->getCount(dim));
	}
}

//-------------------------------------------------------------------
void Context::dbInsertArrayMeta(const uint32_t eventId) {
	for(const auto& it : m_decisions) {
		intern::MemArray<>* array = it.second;

		const uint32_t dims = array->getArray().getVDimCount();
		const uint32_t historyOffset = getCurrentOffsetId(array->getDecisionId()) - array->getOffsetId();
		
		if(dims == 0) {
			event::dbInsertEventData(eventId, array->getEventId(), 0, array->getDecisionId(), historyOffset, 0, 0);
		} else {
			for(uint32_t dim = 0; dim < dims; dim++)
				event::dbInsertEventData(eventId, array->getEventId(), 0, array->getDecisionId(), historyOffset, dim, (float)array->getCount(dim));
		}
	}	
}

//-------------------------------------------------------------------
void Context::dbInsertUserMeta(const uint32_t eventId) {
	for(const auto& usermeta : m_meta)
		event::dbInsertEventData(eventId, 0, 0, usermeta.second.getDecisionId(), 0, 0, usermeta.second.getValue());
}

//-------------------------------------------------------------------
		}
	}
}