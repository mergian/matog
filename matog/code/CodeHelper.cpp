// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeHelper.h"
#include <matog/log.h>
#include <matog/array/Field.h>
#include <matog/array/field/Type.h>
#include <matog/Static.h>
#include <matog/enums.h>

#if WIN32
#pragma warning(disable: 4996)
#endif

using matog::array::Field;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CodeHelper::CodeHelper(const bool isRoot64Bit) : m_isRoot64Bit(isRoot64Bit) {

}

//-------------------------------------------------------------------
Dict* CodeHelper::inc(Dict* dict, const char* name, const char* file) {
	std::ostringstream ss;
	ss << Template::getBase() << file << ".tpl";

	Dict* tmp = dict->AddIncludeDictionary(name);
	tmp->SetFilename(ss.str().c_str());
	return tmp;
}

//-------------------------------------------------------------------
Dict* CodeHelper::incHelper(Dict* dict, const array::Field* p, const bool withRoot, const bool leadingSeparator, const char* tmpl, const char* incl, const char* section, const char* field) {
	Dict* returnDict = 0;

	// travel to root
	if(p->getParent())
		returnDict = incHelper(dict, p->getParent(), withRoot, leadingSeparator, tmpl, incl, section, field);

	// init returnDict in root
	if(!returnDict)
		returnDict = inc(dict, incl, tmpl);

	// add leading separator? ???
	const bool isRoot = p->getParent() == 0;
	const bool isFirstChild = !isRoot && p->getParent()->getParent() == 0;

	if(leadingSeparator && ((withRoot && isRoot) || (!withRoot && isFirstChild)))
		set(returnDict->AddSectionDictionary(section), field, "");

	// add element to list
	if(withRoot || p->getParent())
		set(returnDict->AddSectionDictionary(section), field, p->getName());

	return returnDict;
}

//-------------------------------------------------------------------
void CodeHelper::set(ctemplate::TemplateDictionary* dict, const char* name, const uint64_t value) {
	char buffer[256];
	sprintf(buffer, "%llu", (unsigned long long int)value);
	dict->SetValue(name, buffer);
}

//-------------------------------------------------------------------
void CodeHelper::set(ctemplate::TemplateDictionary* dict, const char* name, const uint32_t value) {
	set(dict, name, (uint64_t)value);
}

//-------------------------------------------------------------------
void CodeHelper::set(ctemplate::TemplateDictionary* dict, const char* name, const char* value) {
	dict->SetValue(name, value);
}

//-------------------------------------------------------------------
void CodeHelper::set(ctemplate::TemplateDictionary* dict, const char* name, const char value) {
	const char buffer[2] = {value, 0};
	dict->SetValue(name, buffer);
}

//-------------------------------------------------------------------
void CodeHelper::setGlobal(ctemplate::TemplateDictionary* dict, const char* name, const uint64_t value) {
	char buffer[256];
	sprintf(buffer, "%llu", (unsigned long long int)value);
	dict->SetTemplateGlobalValue(name, buffer);
}

//-------------------------------------------------------------------
void CodeHelper::setGlobal(ctemplate::TemplateDictionary* dict, const char* name, const uint32_t value) {
	setGlobal(dict, name, (uint64_t)value);
}

//-------------------------------------------------------------------
void CodeHelper::setGlobal(ctemplate::TemplateDictionary* dict, const char* name, const char* value) {
	dict->SetTemplateGlobalValue(name, value);
}

//-------------------------------------------------------------------
void CodeHelper::setGlobal(ctemplate::TemplateDictionary* dict, const char* name, const char value) {
	const char buffer[] = {value, 0};
	dict->SetTemplateGlobalValue(name, buffer);
}

//-------------------------------------------------------------------
	}
}