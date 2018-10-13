// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_KERNEL_HINT_REFHINTS
#define __MATOG_KERNEL_HINT_REFHINTS

#include <matog/dll.h>
#include <matog/enums.h>
#include <map>
#include "Identifier.h"

namespace matog {
	namespace kernel {
		namespace hint {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL RefHints {
	class Identifier2 : public Identifier {
	private:
		uint32_t m_dim;

	public:
		inline Identifier2(const uint32_t arrayId, const uint32_t paramId, const uint32_t dim) : Identifier(arrayId, paramId), m_dim(dim) {
		}

		inline bool operator<(const Identifier2& o) const {
			if(m_arrayId < o.m_arrayId)	return true;
			if(m_arrayId > o.m_arrayId)	return false;
			if(m_paramId < o.m_paramId)	return true;
			if(m_paramId > o.m_paramId)	return false;
			return m_dim < o.m_dim;
		}

		inline uint32_t getDim(void) const {
			return m_dim;
		}
	};

	std::map<Identifier2, Identifier2>	m_mappings;
	std::map<Identifier, uint32_t>		m_index;

public:
	struct Mapping {
		uint32_t arrayId;
		uint32_t paramId;
		uint32_t dim;
		uint32_t index;
	};

	void load(const char* kernelName);
	bool isInitialized(void) const;
	void add(const uint32_t arrayId, const uint32_t paramId, const uint32_t index);
	void store(const char* kernelName);

	Mapping getMapping(const uint32_t arrayId, const uint32_t paramId, const uint32_t dim) const;
};

//-------------------------------------------------------------------
		}
	}
}

#endif