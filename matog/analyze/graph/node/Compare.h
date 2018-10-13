// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYSE_GRAPH_NODE_COMPARE
#define __MATOG_ANALYSE_GRAPH_NODE_COMPARE

#include <matog/analyze.h>

namespace matog {
	namespace analyze {
		namespace graph {
			namespace node {
//-------------------------------------------------------------------
struct Compare { 
	bool operator()(Node* const a, Node* const b) const;
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif