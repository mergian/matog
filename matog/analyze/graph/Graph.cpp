// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <matog/analyze.h>
#include <matog/analyze/graph/Graph.h>
#include <matog/event/Type.h>
#include <matog/event/ProfilingAlgorithm.h>
#include <matog/macros.h>
#include <matog/db/tables.h>
#include <matog/db/Stmt.h>

using matog::db::Stmt;

namespace matog {
	namespace analyze {
		namespace graph {
//-------------------------------------------------------------------
Graph::Graph(void) : 
	m_uid(s_uid++), 
	m_globalOptimal(UINT64_MAX), 
	m_algorithm(event::ProfilingAlgorithm::UNKNOWN) 
{}

//-------------------------------------------------------------------
Graph::~Graph(void) {
}

//-------------------------------------------------------------------
void Graph::add(Node* node) {
	// Get algorithm from first node that is added to this graph.
	// By definition, the profiler has to be equal for the entire
	// graph.
	if(m_nodes.empty()) {
		Stmt stmt("SELECT profiler FROM " DB_PROFILINGS " AS A, " DB_EVENTS " AS B WHERE A.id = B.profiling_id AND B.id = ?;");
		stmt.bind(0, node->getId());

		THROWIF(!stmt.step());
		m_algorithm = stmt.get<event::ProfilingAlgorithm>(0);
	}

	// add node to graph
	m_nodes.emplace(node);

	switch(node->getType()) {
	case event::Type::Call:
		m_calls.emplace((Call*)node);
		return;

	case event::Type::Decision:
		m_decisions.emplace((Decision*)node);
		return;

	default:
		;
	}

	THROW()
}

//-------------------------------------------------------------------
bool Graph::contains(Node* node) {
	return m_nodes.find(node) != m_nodes.end();
}

//-------------------------------------------------------------------
void Graph::exportDot(std::ostream& ss) {
	ss << "subgraph cluster_" << getUID() << " {" << std::endl;
	for(Node* node : m_nodes)
		node->exportDot(ss);
	ss << "}" << std::endl;
}

//-------------------------------------------------------------------
		}
	}
}