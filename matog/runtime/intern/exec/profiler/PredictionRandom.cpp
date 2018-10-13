// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "PredictionRandom.h"
#include <random>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
PredictionRandom::PredictionRandom(Params& params) : Profiler(params) {
}

//-------------------------------------------------------------------
void PredictionRandom::run(void) {	
	const Variant& variant = getKernel().getVariant();
	
	if(variant.count() < 1000000) {
		if(getKernelRunIndex() == 0) {
			for(Hash hash = 0; hash < variant.count(); hash++)
				enqueue(hash);
		} else {
			enqueue(0);
		}
	} else {
		enqueue(0);
	}

	runQueue();
}

//-------------------------------------------------------------------
PredictionRandom::~PredictionRandom(void) {
}

//-------------------------------------------------------------------
				}
			}
		}
	}
}
