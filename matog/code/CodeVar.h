// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_CODEVAR
#define __MATOG_CODE_CODEVAR

#include "Template.h"
#include <stdint.h>
#include <matog/util/String.h>
#include <list>
#include <vector>
#include <fstream>
#include <json/json.h>
#include <matog/Array.h>
#include <matog/array/Field.h>
#include <matog/array/field/VType.h>
#include <ctemplate/template_dictionary.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class CodeVar {
private:	
	// local variables
	array::Field m_root;
	
			char			getVectorIndex							(const uint8_t i) const;
			void			parse									(Json::Value& jRoot, array::Field& field);
			
public:
	// constructor
	CodeVar(Json::Value root);

	// destrutor
	virtual ~CodeVar(void);

	// methods
			void					generate				(void) const;

	// getter / setter
			operator				util::String			(void) const;
};

//-------------------------------------------------------------------
	}
}

#endif