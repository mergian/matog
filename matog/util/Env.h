// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_UTIL_ENV_H
#define __MATOG_UTIL_ENV_H

#include "dll.h"

namespace matog {
	namespace util {
//-------------------------------------------------------------------
/// Helper class to read Environment Variables
class MATOG_UTIL_DLL Env {
	/// prevent initiation
	inline Env(void) {}

public:
	/// set env var
	static void			set		(const char* key, const char* value);

	/// get env var
	static const char*	get		(const char* key);

	/// free env var
	static void			unset	(const char* key);
};
//-------------------------------------------------------------------
	}
}

#endif