// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DEGREE_SET
#define __MATOG_DEGREE_SET

#include <matog/internal.h>
#include <set>

namespace matog {
	namespace degree {
//-------------------------------------------------------------------
struct MATOG_INTERNAL_DLL SetComparator {
	bool operator()(const Degree* const & a, const Degree* const & b) const;
	static int compare(const Degree* const & a, const Degree* const & b);
};

class MATOG_INTERNAL_DLL Set : public std::set<const Degree*, SetComparator> {};
//-------------------------------------------------------------------
	}
}

#endif