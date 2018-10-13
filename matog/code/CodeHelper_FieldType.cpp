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
CodeHelper::FieldType::FieldType(Dict* dict, const array::Field* field, const Params params) {
	m_dict = inc(dict, params.incl ? params.incl : "TYPE", "type")->AddSectionDictionary("TYPE");
	set(m_dict, "TYPE", field->getTypeName());
}

//-------------------------------------------------------------------
	}
}