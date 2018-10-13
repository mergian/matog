// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYSE_GRAPH_GRAPH
#define __MATOG_ANALYSE_GRAPH_GRAPH

#include <matog/util.h>
#include <matog/analyze.h>
#include <vector_types.h>

#include "Call.h"
#include "Decision.h"

#include <ostream>
#include <vector>
#include <fstream>
#include <map>
#include <list>

namespace matog {
	namespace analyze {
		namespace graph {
class Graph {
// STATIC -----------------------------------------------------------
private:
	typedef std::map<Call*, std::map<uint32_t, uint32_t> >	Mapping;
	typedef std::set<const GPU*>							GPUs;

	struct ArrayConfigItem {
		std::list<std::pair<uint32_t, uint32_t> > list;
		const Array* array;
		std::set<uint32_t> categories;
	};

	typedef std::map<uint32_t, ArrayConfigItem> ArrayConfig;
	typedef std::map<const Array*, std::set<uint32_t> > ArrayConfigSet;
	typedef std::map<const Kernel*, std::set<uint64_t> > KernelConfigSet;
	typedef std::map<const Kernel*, std::map<uint32_t, const char*> > UserHashes;

	typedef std::map<const Array*,  util::ml::Type> ArrayModels;
	typedef std::map<const Kernel*, util::ml::Type> KernelModels;

	//---------------------------------------------------------------
	static uint32_t		s_uid;
	static Calls		s_calls;
	static Decisions	s_decisions;
	static Graphs		s_graphs;
	static GPUs			s_gpus;

	static void	loadNodes				(void);
	static void	buildGraphs				(void);
	
public:
	static	void			exec			(void);
	static	void			exportDot		(void);
	static	Call*			getCall			(const uint32_t id);
	static  Graphs&			getAllGraphs	(void);
	static	Decisions&		getAllDecisions	(void);
	static	Calls&			getAllCalls		(void);

// INSTANCE ---------------------------------------------------------
private:
	const uint32_t				m_uid;
	node::Set					m_nodes;
	decision::Set				m_decisions;
	call::Set					m_calls;
	uint64_t					m_globalOptimal;
	event::ProfilingAlgorithm	m_algorithm;

public:
	Graph(void);
	~Graph(void);

			bool						contains	(Node* node);
			void						add			(Node* node);
			void						exportDot	(std::ostream& ss);
	inline	bool						hasDecisions(void) const { return !m_decisions.empty(); }
	inline	uint32_t					getUID		(void) const { return m_uid; }
	inline	const decision::Set&		getDecisions(void) const { return m_decisions; }
	inline	const call::Set&			getCalls	(void) const { return m_calls; }
	inline	event::ProfilingAlgorithm	getAlgorithm(void) const { return m_algorithm; }
};

//-------------------------------------------------------------------
		}
	}
}

#endif