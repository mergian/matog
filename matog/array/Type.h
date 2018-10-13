// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ARRAY_TYPE
#define __MATOG_ARRAY_TYPE

namespace matog {
	namespace array {
//-------------------------------------------------------------------
enum class Type {
	None = 0,		///< Default value
	Global,			///< Global Array (Array::Host, Array::Device)
	Shared,			///< Shared Array (ArrayShared)
	Dyn				///< Dynamic Shared Array (ArrayDynamicShared)
};

//-------------------------------------------------------------------
	}
}

#endif