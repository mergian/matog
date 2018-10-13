// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_CODEMODEL_H
#define __MATOG_CODE_CODEMODEL_H

#include "CodeVar.h"
#include <matog/templates/Singleton.h>
#include <matog/util/String.h>
#include <list>
#include <map>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class CodeModel : public templates::Singleton<CodeModel> {
private:
	static CodeModel* s_instance;

public:
	// typedefs
	typedef std::map<util::String, const CodeVar*> VariableMap_t;

private:
	// variables
	VariableMap_t	m_vars;

	// constructor
	CodeModel(void);

public:
	// destructor
	virtual ~CodeModel(void);

	// methods
	void					addVariable	(const CodeVar* var);
			void			generate	(void) const;
	const 	VariableMap_t&	getVars		(void) const;

	// friends
	friend class templates::Singleton<CodeModel>;
};

//-------------------------------------------------------------------
	}
}

#endif