// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ARRAY_FIELD_MEMORY
#define __MATOG_ARRAY_FIELD_MEMORY

namespace matog {
	namespace array {
		namespace field {
//-------------------------------------------------------------------
enum class Memory {
	GLOBAL = 0,
	SHARED,
	DYN,
	__CNT
};

//-------------------------------------------------------------------
		}
	}
}

#endif