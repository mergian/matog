// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_UTIL_RANDOM_H
#define __MATOG_UTIL_RANDOM_H

#include <cstdint>
#include "dll.h"

namespace matog {
	namespace util {
//-------------------------------------------------------------------
/// Helper class for random numbers
class MATOG_UTIL_DLL Random {
public:
	/// creates U32 random number between min and max
	static	uint32_t	u32	(const uint32_t min, const uint32_t max);
	/// creates S32 random number between min and max
	static	int32_t		s32	(const int32_t min, const int32_t max);
	/// creates float random number between min and max
	static	float		f32	(const float min, const float max);
	/// creates binary random decision
	static  bool		bin (void);
};

//-------------------------------------------------------------------
	}
}

#endif