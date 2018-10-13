// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_CODEHINT
#define __MATOG_CODE_CODEHINT

#include <json/json.h>
#include <cstdint>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class CodeHint {
	void extract(const std::string& in, std::string& arrayName, uint32_t& paramId);

public:
	CodeHint(Json::Value root, const char* kernelName);
};

//-------------------------------------------------------------------
	}
}

#endif