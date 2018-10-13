// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ARRAY_SET
#define __MATOG_ARRAY_SET

#include <matog/internal.h>
#include <set>

namespace matog {
	namespace array {
//-------------------------------------------------------------------
/// Comparator for Set
struct MATOG_INTERNAL_DLL SetComparator {
	bool operator()(const Array* const & a, const Array* const & b) const;
};

/// Set of const Array*, sorted ascending by their id.
class MATOG_INTERNAL_DLL Set : public std::set<const Array*, SetComparator> {};

//-------------------------------------------------------------------
	}
}

#endif