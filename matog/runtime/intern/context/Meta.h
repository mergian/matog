// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_CONTEXT_ITEM
#define __MATOG_RUNTIME_INTERN_CONTEXT_ITEM

#include <matog/runtime.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace context {
//-------------------------------------------------------------------
class Meta {
private:
	const void*					m_ptr;
	const array::field::Type	m_type;
	const uint32_t				m_decisionId;
	const char* const			m_file;
	const int					m_line;

public:
	Meta(const void* ptr, const array::field::Type type, const uint32_t decisionId, const char* const file, const int line);
		
			float				getValue		(void) const;
	inline	array::field::Type	getType			(void) const { return m_type; }
	inline	uint32_t			getDecisionId	(void) const { return m_decisionId; }
	inline	const char*			getFile			(void) const { return m_file; }
	inline	int					getLine			(void) const { return m_line; }
};

//-------------------------------------------------------------------
			}
		}
	}
}
#endif