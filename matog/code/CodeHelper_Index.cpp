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
CodeHelper::Index::Index(Dict* dict, const array::Field* field, const Params params) : m_isRoot64Bit(params.isRoot64Bit) { // const bool noRecursion, const bool onlyVariableCounts, const bool shortNames, const bool isRoot64Bit, const char* name, const char* tmpl) : m_isRoot64Bit(isRoot64Bit) {
	std::function<void (const Field*)> helper = [&](const Field* parent) {
		// start from bottom
		if(parent->getParent() && !params.noRecursion)	helper(parent->getParent());
		else m_dict = inc(dict, params.incl ? params.incl : "INDEX", params.tmpl ? params.tmpl : "index");

		// add dims
		if(parent->isDimensional()) {
			for(char i = 0; i < (char)parent->getDimCount(); i++) {
				if(params.onlyVariableCounts && parent->getCounts()[i] != 0)
					continue;

				Dict* index = m_dict->AddSectionDictionary("INDEX");
				set(index, "TYPE", m_isRoot64Bit && parent->getParent() == 0 ? "uint64_t" : "uint32_t");

				if(params.shortNames) {
					set(inc(index, "NAME", "name")->AddSectionDictionary("NAME"), "NAME", (char)('A' + i));
				} else {
					Name(index, parent).add((char)('A' + i));
				}

				if(params.isCube && parent->isCube())
					break;
			}
		}
	};
	
	helper(field);
}

//-------------------------------------------------------------------
void CodeHelper::Index::add(const char* value) {
	Dict* index = inc(m_dict->AddSectionDictionary("INDEX"), "NAME", "name");
	set(index, "TYPE", "uint32_t");
	set(index->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
void CodeHelper::Index::add(const char value) {
	Dict* index = inc(m_dict->AddSectionDictionary("INDEX"), "NAME", "name");
	set(index, "TYPE", "uint32_t");
	set(index->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
void CodeHelper::Index::add(const uint32_t value) {
	Dict* index = inc(m_dict->AddSectionDictionary("INDEX"), "NAME", "name");
	set(index, "TYPE", "uint32_t");
	set(index->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
void CodeHelper::Index::add(const uint64_t value) {
	Dict* index = inc(m_dict->AddSectionDictionary("INDEX"), "NAME", "name");
	set(index, "TYPE", "uint32_t");
	set(index->AddSectionDictionary("NAME"), "NAME", value);
}

//-------------------------------------------------------------------
	}
}