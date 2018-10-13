// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/util/Exit.h>
#include <mutex>
#include <matog/macros.h>

namespace matog {
	namespace util {
//-------------------------------------------------------------------
Exit exit_instance;

//-------------------------------------------------------------------
Exit::~Exit(void) {
	for(auto& call : m_calls)
		(call)();
}

//-------------------------------------------------------------------
void Exit::add(ExitFunction func) {
	static std::mutex mutex;
	LOCK(mutex);
	exit_instance.m_calls.push_front(func);
}

//-------------------------------------------------------------------
	}
}