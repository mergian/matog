// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_UTIL_EXIT_H
#define __MATOG_UTIL_EXIT_H

#include "dll.h"
#include <list>

namespace matog {
	namespace util {
//-------------------------------------------------------------------
/// Handler class to handling atExit event handlers
class MATOG_UTIL_DLL Exit {
public:
	/// typedef for ExitFunction
	typedef void (*ExitFunction)(void);
		
private:
	/// std::list of ExitFunctions
	class MATOG_UTIL_DLL Calls : public std::list<ExitFunction> {};
			
	/// ExitFunctions to execute at exit
	Calls m_calls;
			
public:
	/// executes the ExitFunctions
	~Exit(void);

	/// add new ExitFunction
	static void add(ExitFunction func);
};

//-------------------------------------------------------------------
	}
}

#endif