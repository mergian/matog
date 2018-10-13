// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_CODEDEFINE
#define __MATOG_CODE_CODEDEFINE

#include <json/json.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class CodeDefine {
public:
	CodeDefine(Json::Value root);
};

//-------------------------------------------------------------------
	}
}

#endif