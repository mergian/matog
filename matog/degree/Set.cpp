// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/degree/Set.h>
#include <matog/Degree.h>
#include <cassert>
#include <cstdint>

namespace matog {
	namespace degree {
//-------------------------------------------------------------------
bool SetComparator::operator()(const Degree* const & a, const Degree* const & b) const {
	return compare(a, b) == 1;
}

//-------------------------------------------------------------------
int SetComparator::compare(const Degree* const & a, const Degree* const & b) {
	assert(a);
	assert(b);

	const static uint32_t PRIORITY[] = {
		UINT32_MAX, // NONE
		0, // L1 Cache
		4, // Layout
		3, // Memory
		5, // Transposition
		1, // Define
		2, // DefineRange
	};

	const uint32_t prioA = PRIORITY[(int)a->getType()];
	const uint32_t prioB = PRIORITY[(int)b->getType()];

	if(prioA < prioB)
		return 1;

	if(prioA > prioB)
		return -1;

	if(a->getId() < b->getId())
		return 1;

	if(a->getId() > b->getId())
		return -1;

	return 0;
}

//-------------------------------------------------------------------
	}
}