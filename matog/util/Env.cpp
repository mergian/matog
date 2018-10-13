// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/util/Env.h>
#include <cstdlib>
#include <cstdio>

#ifdef WIN32
#pragma warning(disable: 4996)
#endif

namespace matog {
	namespace util {
//-------------------------------------------------------------------
void Env::set(const char* key, const char* value) {
#ifdef WIN32
	char buffer[1024];
	sprintf(buffer, "%s=%s", key, value);
	_putenv(buffer);
#else
	setenv(key, value, 1);
#endif
}

//-------------------------------------------------------------------
const char* Env::get(const char* key) {
	return getenv(key);
}

//-------------------------------------------------------------------
void Env::unset(const char* key) {
#ifdef WIN32
	char buffer[1024];
	sprintf(buffer, "%s=", key);
	_putenv(buffer);
#else
	unsetenv(key);
#endif
}

//-------------------------------------------------------------------
	}
}