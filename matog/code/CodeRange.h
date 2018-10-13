// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_CODERANGE
#define __MATOG_CODE_CODERANGE

#include <json/json.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class CodeRange {
public:
	CodeRange(Json::Value root);
};

//-------------------------------------------------------------------
	}
}

#endif