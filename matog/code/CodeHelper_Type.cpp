// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeHelper.h"
#include <matog/log.h>
#include <matog/array/Field.h>
#include <matog/array/field/Type.h>
#include <matog/Static.h>
#include <matog/enums.h>

using matog::array::Field;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CodeHelper::Type::Type(Dict* dict, const array::Field* p, const Params params) { // const bool withRoot, const bool leadingSeparator, const char* incl
	m_dict = incHelper(dict, p, params.withRoot, params.leadingSeparator, "type", params.incl ? params.incl : "TYPE", "TYPE", "TYPE");
}

//-------------------------------------------------------------------
	}
}