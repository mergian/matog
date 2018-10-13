// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ARRAY_FIELD_TYPE
#define __MATOG_ARRAY_FIELD_TYPE

namespace matog {
	namespace array {
		namespace field {
//-------------------------------------------------------------------
enum class Type {
	UNKNOWN = 0,
	U8,
	U16,
	U32,
	U64,
	S8,
	S16,
	S32,
	S64,
	FLOAT,
	DOUBLE,
	STRUCT
};

//-------------------------------------------------------------------
		}
	}
}

#endif