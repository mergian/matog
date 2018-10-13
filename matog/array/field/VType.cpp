// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/internal.h>
#include "VType.h"
#include "Type.h"
#include <matog/log.h>

namespace matog {
	namespace array {
		namespace field {
//-------------------------------------------------------------------
VType::VType(void) : type(Type::UNKNOWN), count(0) {
}

//-------------------------------------------------------------------
VType::VType(const Type _type, const uint32_t _count) : type(_type), count(_count) {
	THROWIF(!isValid(), "Invalid vector type.");
}

//-------------------------------------------------------------------
bool VType::isValid(void) const {
	if(type == Type::DOUBLE)	return false;
	if(type == Type::U64)		return false;
	if(type == Type::S64)		return false;
	if(type == Type::STRUCT)	return false;
	if(type == Type::UNKNOWN)	return false;
	if(count < 2 || count > 4)	return false;
	return true;
}

//-------------------------------------------------------------------
		}
	}
}
