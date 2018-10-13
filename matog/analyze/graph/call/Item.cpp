// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Item.h"
#include <cfloat>

namespace matog {
	namespace analyze {
		namespace graph {
			namespace call {
//-------------------------------------------------------------------
Item::Item(const double _time, const double _occupancy) : time(_time), occupancy(_occupancy) {
}

//-------------------------------------------------------------------
Item::Item(void) : time(DBL_MAX), occupancy(DBL_MAX) {
}

//-------------------------------------------------------------------
bool Item::isValid(void) const {
	return time != DBL_MAX && occupancy != DBL_MAX;
}

//-------------------------------------------------------------------
bool Item::operator<(const Item& other) const {
	return time < other.time;
}

//-------------------------------------------------------------------
			}
		}
	}
}