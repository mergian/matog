// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_KERNEL_HINT_RWHINT
#define __MATOG_KERNEL_HINT_RWHINT

#include <matog/dll.h>
#include <matog/enums.h>
#include <map>
#include "Identifier.h"

namespace matog {
	namespace kernel {
		namespace hint {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL RWHints {
	std::map<Identifier, RWMode> m_modes;

public:
	void load(const char* kernelName);
	RWMode getMode(const uint32_t arrayId, const uint32_t paramId) const;
};

//-------------------------------------------------------------------
		}
	}
}

#endif