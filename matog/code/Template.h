// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_TEMPLATE_H
#define __MATOG_CODE_TEMPLATE_H

#include <ctemplate/template_dictionary.h>
#include <cstdint>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
typedef ctemplate::TemplateDictionary Dict;

//-------------------------------------------------------------------
class Template {
private:
	class UpperCase : public ctemplate::TemplateModifier {
		void Modify(const char* in, size_t inlen, const ctemplate::PerExpandData* per_expand_data, ctemplate::ExpandEmitter* outbuf, const std::string& arg) const;
	};

	class LowerCase : public ctemplate::TemplateModifier {
		void Modify(const char* in, size_t inlen, const ctemplate::PerExpandData* per_expand_data, ctemplate::ExpandEmitter* outbuf, const std::string& arg) const;
	};

	class CamelCase : public ctemplate::TemplateModifier {
		void Modify(const char* in, size_t inlen, const ctemplate::PerExpandData* per_expand_data, ctemplate::ExpandEmitter* outbuf, const std::string& arg) const;
	};

	static UpperCase s_uppercase;
	static LowerCase s_lowercase;
	static CamelCase s_camelcase;

	ctemplate::TemplateDictionary	m_dict;

	void							initTemplateModifiers	(void) const;

protected:
	static std::string s_base;

public:
	Template			(const char* dict);
	virtual ~Template	(void);

	ctemplate::TemplateDictionary*	getDict					(void) const;
	std::string						expandTemplate			(const char* tmpl) const;

	static inline const std::string& getBase(void) { return s_base; }
};

//-------------------------------------------------------------------
	}
}

#endif