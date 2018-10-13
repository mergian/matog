// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <matog/analyze/graph/Call.h>
#include <matog/analyze/graph/Graph.h>
#include <matog/macros.h>
#include <matog/event/Type.h>
#include <matog/event/ProfilingAlgorithm.h>
#include <matog/Static.h>
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/Kernel.h>
#include "call/Item.h"
#include <float.h>

using namespace matog::db;

namespace matog {
	namespace analyze {
		namespace graph {
//-------------------------------------------------------------------
Call::Call(const uint32_t id, const uint32_t kernelId, const uint32_t gpuId, const uint32_t decisionId) : 
	Node			(id, event::Type::Call),
	m_kernel		(&Static::getKernel(kernelId)),
	m_module		(&Static::getModule(m_kernel->getModuleId())),
	m_gpu			(&Static::getGPU(gpuId)),
	m_finalHash		(UINT32_MAX),
	m_decisionId	(decisionId)
{
	assert(m_kernel);
	assert(m_module);
	assert(m_gpu);

	init();
	loadTimes();
}

//-------------------------------------------------------------------
void Call::init(void) {
	Stmt stmt("SELECT DISTINCT source_event_id, param_id FROM " DB_EVENT_DATA " WHERE event_id = ? AND source_event_id != 0;");	
	stmt.bind(0, getId());

	auto& decisions = Graph::getAllDecisions();

	while(stmt.step()) {
		auto it = decisions.find(stmt.get<uint32_t>(0));
		assert(it != decisions.end());

		m_paramIds.emplace(MAP_EMPLACE(&it->second, stmt.get<uint32_t>(1)));
	}
}

//-------------------------------------------------------------------
void Call::loadTimes(void) {
	Stmt stmt("SELECT hash, duration, occupancy FROM " DB_RUNS " WHERE call_id = ?;");
	stmt.bind(0, getId());

	while(stmt.step()) {
		const variant::Hash hash= stmt.get<variant::Hash>(0);
		double duration	= stmt.get<uint64_t>(1) / 1000000.0; // convert to ms
		const double occupancy	= stmt.get<double>(2);

		if(duration == 0.0) {
			L_WARN("duration of 0.0ms detected, possible error?");
			duration = DBL_MAX;
		}
		
		m_items.emplace(MAP_EMPLACE(hash, duration, occupancy));
	}
}

//-------------------------------------------------------------------
Call::~Call(void) {
}

//-------------------------------------------------------------------
void Call::print(void) {
	L_INFO("Call: %s::%s / %u", getModule()->getName(), getKernel()->getName(), getId());
}

//-------------------------------------------------------------------
void Call::exportDot(std::ostream& ss) {
	ss << "N" << getUID() << " [label=\"" << getId() << ":" << getModule()->getName() << "::" << getKernel()->getName() << std::endl;
	
	getKernel()->getVariant().print(getFinalConfig(), ss);

	ss << std::endl;

	for(auto it : m_paramIds) {
		ss << it.first->getArray()->getName() << " --> " << it.first->getId() << ":" << it.second << std::endl;
	}
	
	ss << "\", style=filled, fillcolor=turquoise];" << std::endl;
	ss << std::endl;
	
	Node::exportDot(ss);
}

//-------------------------------------------------------------------
call::Item Call::getItem(const variant::Hash hash) const {
	auto it = m_items.find(hash);
	
	if(it == m_items.end())
		return call::Item();

	return it->second;
}

//-------------------------------------------------------------------
void Call::setFinalConfig(const variant::Hash finalHash) {
	m_finalHash = m_kernel->getVariant().compress(finalHash, variant::Type::Local);
}

//-------------------------------------------------------------------
		}
	}
}