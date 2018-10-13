// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_VARIANT_PRINT
#define __MATOG_VARIANT_PRINT

#include <matog/variant/Item.h>
#include <set>

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
struct MATOG_INTERNAL_DLL PrintComparator { bool operator()(const Item& a, const Item& b) const; };
class MATOG_INTERNAL_DLL Print : public std::set<Item, PrintComparator> {};

//-------------------------------------------------------------------
	}
}

#endif