// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_MODE
#define __MATOG_RUNTIME_INTERN_EXEC_MODE

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
enum class Mode {
	Baseline_SOA,	///< default mode
	Optimized,		///< optimized execution mode
	Profiling,		///< profiling mode
	Baseline_AOS,	///< only AOS
	Baseline_AOSOA	///< only AOSOA + SOA (for sub)
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif