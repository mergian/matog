// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ARRAY_FIELD_VTYPE
#define __MATOG_ARRAY_FIELD_VTYPE

#include <matog/internal.h>

namespace matog {
	namespace array {
		namespace field {
//-------------------------------------------------------------------
struct MATOG_INTERNAL_DLL VType {
	Type type;
	uint32_t count;

	VType(void);
	VType(const Type type, const uint32_t count);

	bool isValid(void) const;
};

//-------------------------------------------------------------------
		}
	}
}

#endif