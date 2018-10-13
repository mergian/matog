// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_KERNEL_HINT_IDENTIFIER
#define __MATOG_KERNEL_HINT_IDENTIFIER

#include <matog/dll.h>
#include <cstdint>

namespace matog {
	namespace kernel {
		namespace hint {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL Identifier {
protected:
	uint32_t m_arrayId;
	uint32_t m_paramId;

public:
	inline Identifier(const uint32_t arrayId, const uint32_t paramId) : m_arrayId(arrayId), m_paramId(paramId) {
	}

	inline bool operator<(const Identifier& o) const {
		if(m_arrayId < o.m_arrayId)	return true;
		if(m_arrayId > o.m_arrayId)	return false;
		return m_paramId < o.m_paramId;
	}

	inline uint32_t getArrayId(void) const { return m_arrayId; }
	inline uint32_t getParamId(void) const { return m_paramId; }
};

//-------------------------------------------------------------------
		}
	}
}

#endif