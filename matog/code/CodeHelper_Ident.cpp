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
CodeHelper::Ident::Ident(Dict* dict, const array::Field* current, const uint32_t dim, const Params params) {
	m_dict = inc(dict, params.incl ? params.incl : "IDENT", "name");
	
	std::function<void (const Field*)> helper = [&](const Field* current) {
		if(!current->isRoot()) {
			helper(current->getParent());
			set(m_dict->AddSectionDictionary("NAME"), "NAME", current->getName());
		}
	};

	if(current)
		helper(current);

	if(current->isDimensional())
		add((char)('A' + dim));
}

//-------------------------------------------------------------------
void CodeHelper::Ident::add(const char* value) {
	set(m_dict->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
void CodeHelper::Ident::add(const char value) {
	set(m_dict->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
void CodeHelper::Ident::add(const uint32_t value) {
	set(m_dict->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
void CodeHelper::Ident::add(const uint64_t value) {
	set(m_dict->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
	}
}