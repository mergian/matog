// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Decisions.h"

#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/Static.h>

#include <matog/macros.h>
#include <matog/log.h>
#include <matog/runtime/intern/Runtime.h>

using namespace matog::db;
using namespace matog::kernel;

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
std::map<Decisions::Ident, uint32_t> Decisions::s_decisions;

//-------------------------------------------------------------------
Decisions::Ident::Ident(db::Stmt& stmt) :
	hash	(stmt.get<uint32_t>(1)),
	arrayId	(stmt.get<uint32_t>(2)),
	kernelId(stmt.get<uint32_t>(3)),
	gpuId	(stmt.get<uint32_t>(4))
{}

//-------------------------------------------------------------------
Decisions::Ident::Ident(const uint32_t _hash, const uint32_t _arrayId, const uint32_t _kernelId, const uint32_t _gpuId) :
	hash	(_hash),
	arrayId	(_arrayId),
	kernelId(_kernelId),
	gpuId	(_gpuId)
{}

//-------------------------------------------------------------------
bool Decisions::Ident::operator<(const Ident& o) const {
	if(hash < o.hash) return true;
	if(hash > o.hash) return false;

	if(arrayId < o.arrayId) return true;
	if(arrayId > o.arrayId) return false;

	if(kernelId < o.kernelId) return true;
	if(kernelId > o.kernelId) return false;

	return gpuId < o.gpuId;
}

//-------------------------------------------------------------------
void Decisions::init(void) {
	Stmt stmt("SELECT id, hash, array_id, kernel_id, gpu_id FROM " DB_DECISIONS ";");

	while(stmt.step())
		s_decisions.emplace(MAP_EMPLACE(stmt, stmt.get<uint32_t>(0)));
}

//-------------------------------------------------------------------
uint32_t Decisions::getId(const uint32_t hash, const char* file, const int line) {
	assert(!Runtime::isBaseline());

	const Ident ident(hash, 0, 0, 0);

	auto it = s_decisions.find(ident);

	if(it == s_decisions.end()) {
		if(!Runtime::isProfiling()) {
			L_ERROR("Unknown decision found: %08X @ %s (%u)", hash, file, line);
			THROW();
		}
		
		const uint32_t id = event::dbInsertDecision(hash, 0, 0, 0, file, line);
		s_decisions.emplace(MAP_EMPLACE(ident, id));
		return id;
	}

	return it->second;
}

//-------------------------------------------------------------------
uint32_t Decisions::getId(const MemArray<>* array, const Kernel* kernel, const GPU* gpu) {
	if(Runtime::isBaseline())
		return 0;

	const Ident ident(
		array ? array->getHash() : 0, 
		array ? array->getArray().getId() : 0, 
		kernel? kernel->getId() : 0, 
		gpu   ? gpu->getId() : 0
	);

	auto it = s_decisions.find(ident);

	if(it == s_decisions.end()) {
		if(!Runtime::isProfiling()) {
			L_ERROR("Unknown decision found: %s --> %s (%s)", array ? array->getArray().getName() : "", kernel ? kernel->getName() : "", gpu ? gpu->getName() : "");
			THROW();
		}

		const uint32_t id = event::dbInsertDecision(ident.hash, ident.arrayId, ident.kernelId, ident.gpuId, array ? array->file() : 0, array ? array->line() : 0);
		s_decisions.emplace(MAP_EMPLACE(ident, id));
		return id;
	}

	return it->second;
}

//-------------------------------------------------------------------
		}
	}
}