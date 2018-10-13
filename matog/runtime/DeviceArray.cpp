// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/runtime/DeviceArray.h>
#include <matog/runtime/intern/DeviceArray.h>
#include <matog/util/Mem.h>

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
DeviceArray::DeviceArray(const uint32_t arrayId, const void* itemPtr, const std::initializer_list<uint64_t>& counts, const char* file, const int line, DeviceMalloc* malloc) :	
		MemArray		(arrayId, true, itemPtr, counts, file, line),
		m_malloc		(malloc),
		m_dataPtr		(0),
		m_device_0		(UINT_MAX)
{}

//-------------------------------------------------------------------
DeviceArray::~DeviceArray(void) {
	if(m_dataPtr)
		allocator()->free(m_dataPtr);
}

//-------------------------------------------------------------------
	}
}