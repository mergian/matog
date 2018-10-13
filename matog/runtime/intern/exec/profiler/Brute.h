// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_RUNTIME_INTERN_EXEC_PROFILER_BRUTE
#define __MATOG_RUNTIME_INTERN_EXEC_PROFILER_BRUTE

#include <matog/runtime/intern/exec/Profiler.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
class Brute : public Profiler {
public:
										Brute	(Params& params);	
	virtual								~Brute	(void);
	virtual	void						run		(void);
};

//-------------------------------------------------------------------
				}
			}
		}
	}
}

#endif