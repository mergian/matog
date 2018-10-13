// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Compare.h"
#include "../Node.h"

namespace matog {
	namespace analyze {
		namespace graph {
			namespace node {
//-------------------------------------------------------------------
bool Compare::operator()(Node* const a, Node* const b) const {
	return a->getUID() < b->getUID(); 
}

//-------------------------------------------------------------------
			}
		}
	}
}