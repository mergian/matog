// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_VARIANT_ITEMS
#define __MATOG_VARIANT_ITEMS

#include <matog/variant/Item.h>
#include <vector>
#include <set>

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
/// List of Items
class MATOG_INTERNAL_DLL Items : public std::set<Item> {};

//-------------------------------------------------------------------
	}
}

#endif