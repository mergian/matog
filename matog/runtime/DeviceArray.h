// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_DEVICEARRAY
#define __MATOG_RUNTIME_DEVICEARRAY

#include <matog/runtime/MemArray.h>
#include <vector>

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
// forward declaration
class MATOG_RUNTIME_DLL HostArray;

/**
	Base Device array class
*/
class MATOG_RUNTIME_DLL DeviceArray : public MemArray {
protected:
	DeviceMalloc*			m_malloc;				///< allocator
	CUdeviceptr				m_dataPtr;				///< data pointer

	uint32_t				m_device_0;				///< internal use only

	/// default constructor
	DeviceArray(const uint32_t arrayId, const void* itemPtr, const std::initializer_list<uint64_t>& counts, const char* file, const int line, DeviceMalloc* malloc);

public:
	/// destructor
	virtual	~DeviceArray(void);

	/// returns the used allocator
	inline DeviceMalloc* allocator(void) const { return m_malloc; }

	/// returns the data pointer. WARNING: not intended to be used directly!
	inline CUdeviceptr data_ptr(void) const { return m_dataPtr; }

	/// creates a clone of the array, with the same sizes, but does not copy the data
	virtual	DeviceArray* clone(const char* file, const int line, DeviceMalloc* malloc = &DefaultDeviceMalloc::getInstance()) const = 0;

	/// create a host-clone of the array, with the same sizez, but does not copy the data
	virtual	HostArray* clone_host(const char* file, const int line, HostMalloc* malloc = &DefaultHostMalloc::getInstance()) const = 0;

	/// conversion operator
	inline operator DeviceArray*(void) const { return (DeviceArray*)this; }
};

//-------------------------------------------------------------------
	}
}

#endif