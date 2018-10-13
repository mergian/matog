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
CodeHelper::Name::Name(Dict* dict, const array::Field* p, const Params params) { // , const bool withRoot, const bool leadingSeparator, const char* incl
	m_dict = incHelper(dict, p, params.withRoot, params.leadingSeparator, "name", params.incl ? params.incl : "NAME", "NAME", "NAME");
}

//-------------------------------------------------------------------
void CodeHelper::Name::add(const char* value) {
	set(m_dict->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
void CodeHelper::Name::add(const char value) {
	set(m_dict->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
void CodeHelper::Name::add(const uint32_t value) {
	set(m_dict->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
void CodeHelper::Name::add(const uint64_t value) {
	set(m_dict->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
	}
}