// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYSE_GRAPH_CALL_ITEM
#define __MATOG_ANALYSE_GRAPH_CALL_ITEM

#include <matog/analyze.h>

namespace matog {
	namespace analyze {
		namespace graph {
			namespace call {
//-------------------------------------------------------------------
struct Item {
	double time;
	double occupancy;
	
	Item(void);
	Item(const double _time, const double _occupancy);
	
	bool isValid(void) const;

	bool operator<(const Item& other) const;
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif