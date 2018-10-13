// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Meta.h"
#include <matog/array/field/Type.h>
#include <matog/log.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace context {
//-------------------------------------------------------------------
using array::field::Type;

//-------------------------------------------------------------------
Meta::Meta(const void* ptr, const Type type, const uint32_t decisionId, const char* const file, const int line) :
	m_ptr			(ptr),
	m_type			(type),
	m_decisionId	(decisionId),
	m_file			(file),
	m_line			(line)
{}

//-------------------------------------------------------------------
float Meta::getValue(void) const {
	switch(getType()) {
		case Type::S8:		return (float)*(int8_t*)m_ptr;
		case Type::S16:		return (float)*(int16_t*)m_ptr;
		case Type::S32:		return (float)*(int32_t*)m_ptr;
		case Type::S64:		return (float)*(int64_t*)m_ptr;
		case Type::U8:		return (float)*(uint8_t*)m_ptr;
		case Type::U16:		return (float)*(uint16_t*)m_ptr;
		case Type::U32:		return (float)*(uint32_t*)m_ptr;
		case Type::U64:		return (float)*(uint64_t*)m_ptr;
		case Type::FLOAT:	return *(float*)m_ptr;
		case Type::DOUBLE:	return (float)*(double*)m_ptr;
		default: ;
	}

	THROW()
	return 0.0f;
}

//-------------------------------------------------------------------
			}
		}
	}
}