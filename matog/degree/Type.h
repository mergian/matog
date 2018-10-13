// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DEGREE_TYPE
#define __MATOG_DEGREE_TYPE

namespace matog {
	namespace degree {
//-------------------------------------------------------------------
enum class Type {
	None = 0,
	L1Cache,
	ArrayLayout,
	ArrayMemory,
	ArrayTransposition,
	Define,
	Range
};

//-------------------------------------------------------------------
	}
}

#endif