// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/runtime/BaseArray.h>
#include <matog/runtime/intern/Runtime.h>
#include <matog/runtime/intern/BaseArray.h>


namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
uint64_t BaseArray::textureAlign(uint64_t a) {
	return align(a, 512); // tex memory is supposed to be 512B aligned!
}

//-------------------------------------------------------------------
uint64_t BaseArray::align(uint64_t a, size_t _alignment) {
	const uint64_t alignment = (uint64_t)_alignment;
	return ((a + alignment - 1) / alignment) * alignment;
} 

//-------------------------------------------------------------------
uint32_t BaseArray::align(uint32_t a, size_t _alignment) {
	const uint32_t alignment = (uint32_t)_alignment;
	return ((a + alignment - 1) / alignment) * alignment;
} 

//---------------------------------------------------------------
BaseArray::BaseArray(
	const uint32_t arrayId,
	const bool isDevice,
	const void* const itemPtr, 
	const std::initializer_list<uint64_t>& counts,
	const char* file, 
	const int line) 
: 
	m_file		(file),
	m_line		(line),
	m_array_0	(itemPtr),
	m_array_1	(0),
	m_array_2	(0),
	m_array_7	(0)
{
	if(counts.size() > 0) {
		uint32_t dim = 0;
		m_array_7 = NEW_A(uint64_t, counts.size());
		for(const uint64_t value : counts)
			m_array_7[dim++] = value;
	}

	intern::Runtime::registerArray((intern::BaseArray<>*)this, arrayId, isDevice);
}

//-------------------------------------------------------------------
BaseArray::~BaseArray(void) {
	intern::Runtime::deregisterArray((intern::BaseArray<>*)this);

	if(m_array_7)
		FREE_A(m_array_7);
}

//-------------------------------------------------------------------
	}
}