// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Brute.h"
#include <matog/runtime/intern/Runtime.h>
#include <matog/variant/Type.h>
#include <matog/cuda/Architecture.h>
#include <matog/util.h>
#include <matog/log.h>

using namespace matog::variant;

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
Brute::Brute(Params& params) : Profiler(params) {
}

//-------------------------------------------------------------------
void Brute::run(void) {
	// enqueue all
	const Hash configCnt = getKernel().getVariant().count();

	L_DEBUG("Max Configuration Count: %llu", (unsigned long long int)configCnt);

	for(Hash i = 0; i < configCnt; i++) {
		// Run partially. Otherwise the queue will be extremely long!
		if(i % 1000000 == 0)
			runQueue();
		enqueue(i);
	}

	// run all
	runQueue();
	
	// compare to baseline
	L_DEBUG("%f times faster than baseline", (getDuration(0) / (float)getBestDuration()));
}

//-------------------------------------------------------------------
Brute::~Brute(void) {
}

//-------------------------------------------------------------------
				}
			}
		}
	}
}
