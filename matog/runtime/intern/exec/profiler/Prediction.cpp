// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Prediction.h"

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
Prediction::Prediction(Params& params) : Profiler(params) {
}

//-------------------------------------------------------------------
void Prediction::run(void) {
	variant::List list;
	getKernel().getVariant().predictionSet(list);

	for(const Hash hash : list)
		enqueue(hash);

	runQueue();
}

//-------------------------------------------------------------------
Prediction::~Prediction(void) {
}

//-------------------------------------------------------------------
				}
			}
		}
	}
}
