// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/util/Random.h>
#include <cassert>
#include <cstdlib>

namespace matog {
	namespace util {
//-------------------------------------------------------------------
float Random::f32(const float min, const float max) {
	float r = min + rand() * (max - min + 1)/(float)RAND_MAX;

	// compensate possible floating point inaccuracy
	if(r < min)
		r = min;

	if(r > max)
		r = max;

	return r;
}

//-------------------------------------------------------------------
uint32_t Random::u32(const uint32_t min, const uint32_t max) {
	if(max == min)
		return max;

	uint32_t r = min + (rand() % (max - min));
	assert(r >= min);
	assert(r < max);
	return r;
}

//-------------------------------------------------------------------
int32_t Random::s32(const int32_t min, const int32_t max) {
	if(max == min)
		return max;

	int32_t r = min + (rand() % (max - min));
	assert(r >= min);
	assert(r < max);
	return r;
}

//-------------------------------------------------------------------
bool Random::bin(void) {
	return u32(0, 1) == 0;
}

//-------------------------------------------------------------------
	}
}


