// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/analyze/graph/Graph.h>
#include <matog/log.h>
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/event/Type.h>
#include <matog/Options.h>
#include <matog/util.h>
#include <matog/macros.h>

using namespace matog::db;

namespace matog {
	namespace analyze {
		namespace graph {
//-------------------------------------------------------------------
uint32_t	Graph::s_uid = 0;
Calls		Graph::s_calls;
Decisions	Graph::s_decisions;
Graphs		Graph::s_graphs;
Graph::GPUs	Graph::s_gpus;

//-------------------------------------------------------------------
Call* Graph::getCall(const uint32_t id) {
	auto it = s_calls.find(id);
	assert(it != s_calls.end());
	return &it->second;
}

//-------------------------------------------------------------------
void Graph::exportDot(void) {
	Json::Value json = Options::get<Json::Value>({"analyze", "dot"}, Json::Value::null);
	
	if(json.isString()) {
		const std::string filename(json.asString());

		C_INFO(log::GREEN, "-- exporting graphs");
		std::ofstream ss(filename.c_str());
		ss << std::endl << "digraph G {" << std::endl;
		ss << "graph [splines=polyline];" << std::endl;
		ss << "node [shape=rect, fontname=consolas];" << std::endl;

		for(auto& item : s_graphs)	
			item.exportDot(ss);

		ss << "}" << std::endl;
	}
}

//-------------------------------------------------------------------
void Graph::exec(void) {
	C_INFO(log::GREEN, "-- building graphs");
	loadNodes();
	buildGraphs();
}

//-------------------------------------------------------------------
void Graph::loadNodes(void) {
	// load decisions
	{
		L_INFO("loading array decision nodes...");
		Stmt stmt("SELECT A.id, B.id, B.array_id, B.kernel_id, B.gpu_id FROM " DB_EVENTS " AS A, " DB_DECISIONS " AS B WHERE A.decision_id = B.id AND B.array_id != 0;");

		while(stmt.step()) {
			s_decisions.emplace(MAP_EMPLACE(
				stmt.get<uint32_t>(0),
				stmt.get<uint32_t>(0),
				stmt.get<uint32_t>(1),
				stmt.get<uint32_t>(2),
				stmt.get<uint32_t>(3),
				stmt.get<uint32_t>(4)
			));
		}
	}

	// load calls
	{
		L_INFO("loading call decision nodes...");
		Stmt stmt("SELECT A.id, B.kernel_id, B.gpu_id, B.id FROM " DB_EVENTS " AS A, " DB_DECISIONS " AS B WHERE A.decision_id = B.id AND B.array_id = 0 AND B.kernel_id != 0;");

		while(stmt.step()) {
			s_calls.emplace(MAP_EMPLACE(
				stmt.get<uint32_t>(0),
				stmt.get<uint32_t>(0),
				stmt.get<uint32_t>(1),
				stmt.get<uint32_t>(2),
				stmt.get<uint32_t>(3)
			));
		}
	}

	// connect calls to decisions
	{
		Stmt stmt("SELECT DISTINCT source_event_id FROM " DB_EVENT_DATA " WHERE event_id = ? AND source_event_id != 0;");

		for(auto& it : s_calls) {
			Call* call = &it.second;

			stmt.reset();
			stmt.bind(0, call->getId());

			while(stmt.step()) {
				auto it = s_decisions.find(stmt.get<uint32_t>(0));
				THROWIF(it == s_decisions.end());

				// connect
				Decision* decision = &it->second;
				decision->getLast()->setChild(decision, call);
				call->setParent(decision, decision->getLast());
				decision->setLast(call);
			}
		}
	}
}

//-------------------------------------------------------------------
void Graph::buildGraphs(void) {
	L_INFO("building graphs...");
	node::Set nodes;

	for(auto& item : s_calls)		nodes.emplace(&item.second);
	for(auto& item : s_decisions)	nodes.emplace(&item.second);

	while(!nodes.empty()) {
		s_graphs.emplace_back();
		Graph& graph = s_graphs.back();

		std::function<void (Node*)> func = [&](Node* current) {
			// remove current
			nodes.erase(current);
			graph.add(current);

			// traverse all parents
			for(auto& parent : current->getParents()) {
				if(nodes.find(parent.second) != nodes.end())
					func(parent.second);
			}

			// traverse all children
			for(auto& child : current->getChildren()) {
				if(nodes.find(child.second) != nodes.end())
					func(child.second);
			}
		};

		func(*nodes.begin());
	}
}

//-------------------------------------------------------------------
Graphs& Graph::getAllGraphs(void) {
	return s_graphs;
}

//-------------------------------------------------------------------
Decisions& Graph::getAllDecisions(void) {
	return s_decisions;
}

//-------------------------------------------------------------------
Calls& Graph::getAllCalls(void) {
	return s_calls;
}

//-------------------------------------------------------------------
		}
	}
}