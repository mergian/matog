// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_VARIANT_SET
#define __MATOG_VARIANT_SET

#include <matog/variant/Item.h>
#include <set>

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL Set : public std::set<Item> {};

//-------------------------------------------------------------------
	}
}

#endif