// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Plain.h"

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec { 
				namespace profiler {
//-------------------------------------------------------------------
Plain::Plain(Params& params) : Profiler(params)	{
}

//-------------------------------------------------------------------
Plain::~Plain(void) {
}

//-------------------------------------------------------------------
void Plain::run(void) {
	enqueue(0);
	runQueue();
}

//-------------------------------------------------------------------
				}
			}
		}
	}
}