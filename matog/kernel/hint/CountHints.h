// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_KERNEL_HINT_COUNTHINTS
#define __MATOG_KERNEL_HINT_COUNTHINTS

#include <matog/dll.h>
#include <matog/enums.h>
#include <map>
#include <vector>
#include "Identifier.h"

namespace matog {
	namespace kernel {
		namespace hint {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL CountHints {
private:
	std::map<Identifier, std::vector<uint64_t> > m_counts;

public:
	void load(const char* kernelName);
	const uint64_t* getCounts(const uint32_t arrayId, const uint32_t paramId) const;
};

//-------------------------------------------------------------------
		}
	}
}

#endif