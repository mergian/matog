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
CodeHelper::Values::Values(Dict* dict, const array::Field* field, const Params params) { // const bool noRecursion, const bool onlyVariableCounts, const bool shortNames, const char* name, const char* tmpl
	std::function<void (const Field*)> helper = [&](const Field* parent) {

		// start from bottom
		if(parent->getParent() && !params.noRecursion)	helper(parent->getParent());
		else											m_dict = inc(dict, params.incl ? params.incl : "VALUES", params.tmpl ? params.tmpl : "values");

		// add dims
		if(parent->isDimensional()) {
			for(char i = 0; i < (char)parent->getDimCount(); i++) {
				if(params.onlyVariableCounts && parent->getCounts()[i] != 0)
					continue;

				if(params.shortNames) {
					add((char)('A' + i));
				} else {
					Dict* values = m_dict->AddSectionDictionary("VALUES");
					if(!params.lastNoChar || (uint32_t)(i+1) < parent->getDimCount())
						values->ShowSection("IS_CHAR");
					Name name(values, parent);
					name.add((char)('A' + i));
				}

				if(params.isCube && parent->isCube())
					break;
			}
		}
	};
	
	helper(field);
}

//-------------------------------------------------------------------
void CodeHelper::Values::add(const char* value) {
	Dict* dict = inc(m_dict->AddSectionDictionary("VALUES"), "NAME", "name")->AddSectionDictionary("NAME");
	set(dict, "NAME", value);
	dict->ShowSection("IS_CHAR");
}

//-------------------------------------------------------------------
void CodeHelper::Values::add(const char value) {
	Dict* dict = inc(m_dict->AddSectionDictionary("VALUES"), "NAME", "name")->AddSectionDictionary("NAME");
	set(dict, "NAME", value);
	dict->ShowSection("IS_CHAR");
}

//-------------------------------------------------------------------
void CodeHelper::Values::add(const uint32_t value) {
	Dict* dict = inc(m_dict->AddSectionDictionary("VALUES"), "NAME", "name")->AddSectionDictionary("NAME");
	set(dict, "NAME", value);
	dict->ShowSection("IS_NUMERIC");
}

//-------------------------------------------------------------------
void CodeHelper::Values::add(const uint64_t value) {
	Dict* dict = inc(m_dict->AddSectionDictionary("VALUES"), "NAME", "name")->AddSectionDictionary("NAME");
	set(dict, "NAME", value);
	dict->ShowSection("IS_NUMERIC");
}

//-------------------------------------------------------------------
	}
}