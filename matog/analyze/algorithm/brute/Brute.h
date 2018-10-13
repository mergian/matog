// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYSE_ALGORITHM_BRUTE_BRUTE
#define __MATOG_ANALYSE_ALGORITHM_BRUTE_BRUTE

#include <matog/analyze.h>
#include <matog/analyze/graph/Decision.h>
#include <matog/variant/Items.h>
#include <matog/Variant.h>
#include <unordered_map>
#include <vector>

namespace matog {
	namespace analyze {
		namespace algorithm {
			namespace brute {
//-------------------------------------------------------------------
class Brute {
private:
	typedef std::unordered_map<graph::Call*, std::vector< std::pair<variant::Hash, double> > >	TimeMap;
	typedef std::map<graph::Decision*, uint32_t>												DecisionIds;

	const graph::Graph&	m_graph;
	TimeMap				m_calls;
	DecisionIds			m_decisionIds;
	variant::Items		m_items;
	Variant				m_variants;

	void initCalls(void);
	void initCallBrute		(TimeMap::iterator it);
	void initCallPrediction	(TimeMap::iterator it);
	void initVariants		(void);
	void process			(void);
	void setBest			(const variant::Hash hash);

	variant::CompressedHash getGlobalHash(const variant::Hash hash, const graph::Call* call) const;

public:
	Brute(const graph::Graph& graph);
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif