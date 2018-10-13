// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYZE_TRAINING_TRAINING_H
#define __MATOG_ANALYZE_TRAINING_TRAINING_H

#include <cstdint>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <matog/DB.h>

namespace matog {
	namespace analyze {
		namespace training {
//-------------------------------------------------------------------
class Training {
	static void arrays(void);
	static void calls(void);

public:
	static void exec(void);
	static bool isLog(void);
};

//-------------------------------------------------------------------
		}
	}
}

#endif