// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYZE
#define __MATOG_ANALYZE

#include <matog/internal.h>
#include <map>
#include <set>
#include <list>

namespace matog {
	namespace analyze {
		class Analzye;

		namespace algorithm {
			class Algorithm;
			enum class Type;

			namespace brute {
				class Brute;
				class Config;
				class Data;
			}

			namespace prediction {
				class Prediction;
				class Helper;
			}
		}

		namespace graph {
			class Call;
			class Decision;
			class Graph;
			class Node;

			typedef std::map<uint32_t, Call>		Calls;
			typedef std::map<uint32_t, Decision>	Decisions;
			typedef std::list<Graph>				Graphs;

			namespace node {
				struct Compare;
				typedef std::map<Decision*, Node*, Compare> Map;
				typedef std::set<Node*, Compare> Set;
			}

			namespace call { 
				struct Item;
				typedef std::set<Call*, node::Compare> Set; 
				typedef std::map<variant::Hash, Item> ItemMap;
				typedef std::multimap<Decision*, uint32_t> ParamIds;
			}

			namespace decision { 
				typedef std::set<Decision*, node::Compare>	Set; 
			}
		}

		namespace training {
			class Training;
			class Data;
		}

		namespace compiler {
		
		}
	}
}
		
#endif