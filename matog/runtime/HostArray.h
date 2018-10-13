// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_HOSTARRAY
#define __MATOG_RUNTIME_HOSTARRAY

#include <matog/runtime/MemArray.h>

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
// forward declaration
class MATOG_RUNTIME_DLL DeviceArray;

/**
	Base Host array implementation
*/
class MATOG_RUNTIME_DLL HostArray : public MemArray {
protected:
	HostMalloc*	m_malloc;	///< allocator
	void*		m_dataPtr;	///< data pointer

	/// default constructor
	HostArray(const uint32_t arrayId, const void* itemPtr, const std::initializer_list<uint64_t>& counts, const char* file, const int line, HostMalloc* allocator, void* dataPtr = 0);

public:
	/// destructor
	virtual ~HostArray(void);

	/// returns the used allocator
	inline HostMalloc* allocator(void) const { return m_malloc; }

	/// returns the data pointer. WARNING: not intended to be used directly!
	inline void* data_ptr(void) const { return m_dataPtr; }
	
	/// creates a clone of the array, with the same sizes, but does not copy the data
	virtual HostArray* clone(const char* file, const int line, HostMalloc* malloc = &DefaultHostMalloc::getInstance()) const = 0;
	
	/// create a device-clone of the array, with the same sizez, but does not copy the data
	virtual	DeviceArray* clone_device(const char* file, const int line, DeviceMalloc* malloc = &DefaultDeviceMalloc::getInstance()) const = 0;

	/// copies data from source into this array
	virtual void copy(const HostArray* source) = 0;

	/// compares the data in this and the other array
	virtual bool compare(const HostArray* other) = 0;

	/// conversion operator
	inline operator HostArray*(void) const { return (HostArray*)this; }
};

//-------------------------------------------------------------------
	}
}

#endif