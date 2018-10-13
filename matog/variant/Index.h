// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_VARIANT_INDEX
#define __MATOG_VARIANT_INDEX

#include <map>
#include <cstdint>
#include <matog/variant/Item.h>

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
class Index : public std::map<Item, uint32_t> {};

//-------------------------------------------------------------------
	}
}

#endif