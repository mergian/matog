// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <cassert>
#include <matog/array/Set.h>
#include <matog/Array.h>

namespace matog {
	namespace array {
//-------------------------------------------------------------------
bool SetComparator::operator()(const Array* const & a, const Array* const & b) const {
	assert(a);
	assert(b);
	return a->getId() < b->getId();
}

//-------------------------------------------------------------------
	}
}