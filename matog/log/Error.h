// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_LOG_ERROR_H
#define __MATOG_LOG_ERROR_H

#include <cassert>
#include "Result.h"

/// Throws an Exception
#define THROW(...)			{ matog::log::Result(__VA_ARGS__).setFL(__FILE__, __LINE__).print(); assert(false); abort(); exit(1); }

// other stuff
#define XSTRINGIFY(...) STRINGIFY(__VA_ARGS__)
#define STRINGIFY(...) #__VA_ARGS__

/// Checks if the command executes normally, otherwise throws an Exception
#define CHECK(...)			{ matog::log::Result __result(__VA_ARGS__); if(!__result.isSuccess()) THROW(__result) }

/// Throws an Exception if cond is true
#define THROWIF(cond, ...)	if(cond) THROW(__VA_ARGS__)

#endif