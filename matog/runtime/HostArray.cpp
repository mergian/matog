// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/runtime/HostArray.h>

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
HostArray::HostArray(const uint32_t arrayId, const void* itemPtr, const std::initializer_list<uint64_t>& counts, const char* file, const int line, HostMalloc* allocator, void* dataPtr) :
	MemArray	(arrayId, false, itemPtr, counts, file, line),
	m_malloc	(allocator),
	m_dataPtr	(dataPtr)
{}

//-------------------------------------------------------------------
HostArray::~HostArray(void) {
	if(m_dataPtr)
		allocator()->free(m_dataPtr);
}

//-------------------------------------------------------------------
	}
}