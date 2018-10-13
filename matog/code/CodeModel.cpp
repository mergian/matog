// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeModel.h"
#include <matog/log.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CodeModel* CodeModel::s_instance = 0;

//-------------------------------------------------------------------
CodeModel::CodeModel(void) {
}

//-------------------------------------------------------------------
CodeModel::~CodeModel(void) {
	for(auto& item : m_vars)
		FREE(item.second);
}

//-------------------------------------------------------------------
const CodeModel::VariableMap_t&	CodeModel::getVars(void) const {
	return m_vars;
}

//-------------------------------------------------------------------
void CodeModel::generate(void) const {
	L_INFO("Generating code...");

	// iterate all paths
	for(auto& item : m_vars) {
		// get variable
		const CodeVar* var = item.second;
		assert(var);
			
		// generate class structure
		var->generate();
	}
}

//-------------------------------------------------------------------
void CodeModel::addVariable(const CodeVar* var) {
	// check if the variable already exists
	VariableMap_t::iterator varIt = m_vars.find(*var);

	THROWIF(varIt != m_vars.end(), "CODE_VARIABLE_ALREADY_EXISTS");

	// add variable to variable map
	m_vars[*var] = var;
}

//-------------------------------------------------------------------
	}
}