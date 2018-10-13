// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/cuda/jit/Item.h>

namespace matog {
	namespace cuda {
		namespace jit {
//-------------------------------------------------------------------
bool Item::operator<(const Item& other) const {
	if(getModuleId() < other.getModuleId()) return true;
	if(getModuleId() > other.getModuleId()) return false;
                                                         
	if(getCC() < other.getCC()) return true;
	if(getCC() > other.getCC()) return false;

	if(getModuleHash() < other.getModuleHash()) return true;
	if(getModuleHash() > other.getModuleHash()) return false;

	if(getUserHash() < other.getUserHash()) return true;
	if(getUserHash() > other.getUserHash()) return false;

	if(getArch() < other.getArch()) return true;
	if(getArch() > other.getArch()) return false;

	return false;
}

//-------------------------------------------------------------------
		}
	}
}