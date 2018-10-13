// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file. 
#ifndef __{{GLOBAL_NAME:x-uppercase}}_TYPE
#define __{{GLOBAL_NAME:x-uppercase}}_TYPE

#include <cstdint>

struct {{GLOBAL_NAME}}Type {
	{{>UNION}}
	{{>STRUCT}}
	{{>UNION_POINTER}}
};

#endif