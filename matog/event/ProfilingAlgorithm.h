// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_EVENT_PROFILINGALGORITHM
#define __MATOG_EVENT_PROFILINGALGORITHM

namespace matog {
	namespace event {
//-------------------------------------------------------------------
enum class ProfilingAlgorithm {
	UNKNOWN = 0,
	Plain,
	Prediction,
	Brute,
	PredictionRandom
};

//-------------------------------------------------------------------
	}
}

#endif