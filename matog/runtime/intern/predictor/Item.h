// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_PREDICTOR_ITEM
#define __MATOG_RUNTIME_INTERN_PREDICTOR_ITEM

#include <cstdint>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace predictor {
//-------------------------------------------------------------------
class Item {
	uint32_t m_decisionId;
	uint32_t m_offset;
	uint32_t m_dim;

public:
	inline Item(const uint32_t decisionId, const uint32_t offset, const uint32_t dim) :
		m_decisionId		(decisionId),
		m_offset			(offset),
		m_dim				(dim)
	{}

	inline uint32_t decisionId	(void) const	{ return m_decisionId; }
	inline uint32_t offset		(void) const	{ return m_offset; }
	inline uint32_t dim			(void) const	{ return m_dim; }
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif