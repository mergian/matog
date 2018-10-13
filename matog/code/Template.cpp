// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Template.h"
#include <matog/log.h>
#include <matog/util/IO.h>
#include <matog/util/String.h>
#include <ctemplate/template.h>

using matog::util::String;
using matog::util::IO;

#if WIN32
#pragma warning(disable: 4996)
#endif

namespace matog {
	namespace code {
//-------------------------------------------------------------------
Template::UpperCase Template::s_uppercase;
Template::LowerCase Template::s_lowercase;
Template::CamelCase Template::s_camelcase;
std::string Template::s_base;

//-------------------------------------------------------------------
void Template::UpperCase::Modify(const char* in, size_t inlen, const ctemplate::PerExpandData* per_expand_data, ctemplate::ExpandEmitter* outbuf, const std::string& arg) const {
	outbuf->Emit(String(in).toUpperCase().c_str());
}
		
//-------------------------------------------------------------------
void Template::LowerCase::Modify(const char* in, size_t inlen, const ctemplate::PerExpandData* per_expand_data, ctemplate::ExpandEmitter* outbuf, const std::string& arg) const {
	outbuf->Emit(String(in).toLowerCase().c_str());
}

//-------------------------------------------------------------------
void Template::CamelCase::Modify(const char* in, size_t inlen, const ctemplate::PerExpandData* per_expand_data, ctemplate::ExpandEmitter* outbuf, const std::string& arg) const {
	outbuf->Emit(String(in).toCamelCase().c_str());
}

//-------------------------------------------------------------------
std::string Template::expandTemplate(const char* tmpl) const {
	std::string tmp;
	
	std::ostringstream ss;
	ss << s_base << tmpl << ".tpl";

	const std::string file = ss.str();

	THROWIF(!ctemplate::ExpandTemplate(file.c_str(), 
		ctemplate::DO_NOT_STRIP, 
		&m_dict, 
		&tmp), "CODE_CANNOT_EXPAND_TEMPLATE");

	return tmp;
} 

//-------------------------------------------------------------------
void Template::initTemplateModifiers(void) const {
	ctemplate::AddModifier("x-uppercase", &s_uppercase);
	ctemplate::AddModifier("x-lowercase", &s_lowercase);
	ctemplate::AddModifier("x-camelcase", &s_camelcase);
}

//-------------------------------------------------------------------
ctemplate::TemplateDictionary* Template::getDict(void) const {
	// TODO: no nice solution...
	return (ctemplate::TemplateDictionary*)&m_dict;
}

//-------------------------------------------------------------------
Template::Template(const char* dict) : m_dict(dict) {
	if(s_base.empty()) {
		s_base = util::IO::getExecutablePath().c_str();
		s_base.append("tpl/");
	}

	initTemplateModifiers();
}

//-------------------------------------------------------------------
Template::~Template(void) {
}

//-------------------------------------------------------------------
	}
}