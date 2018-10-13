// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_MALLOC
#define __MATOG_RUNTIME_MALLOC

#include <matog/runtime/dll.h>
#include <cuda.h>
#include <matog/log/Result.h>

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
/**
	Default allocator template
*/
template<typename T>
class Malloc {
public:
	/// allocate array of type T, with size bytes
	virtual void malloc(T* ptr, const size_t bytes) = 0;

	/// free pointer
	virtual void free(T ptr) = 0;
};

/// typedef device allocator
typedef Malloc<CUdeviceptr> DeviceMalloc;

/// typedef host allocator
typedef Malloc<void*> HostMalloc;

/**
	Default device allocator, which uses cuMemAlloc and cuMemFree
*/
class MATOG_RUNTIME_DLL DefaultDeviceMalloc : public DeviceMalloc {
private:
	static DefaultDeviceMalloc s_instance;	///< static allocator instance

public:
	/// allocates an array of size bytes
	virtual void malloc	(CUdeviceptr* ptr, const size_t bytes);

	/// frees device pointer
	virtual void free	(CUdeviceptr ptr);

	/// returns the default instance
	static DefaultDeviceMalloc& getInstance(void);
};

/**
	Default host allocator, which uses ::malloc and ::free
*/
class MATOG_RUNTIME_DLL DefaultHostMalloc : public HostMalloc {
private:
	static DefaultHostMalloc s_instance;	///< static allocator instance

public:
	/// allocates an array of size bytes
	virtual void malloc	(void** ptr, const size_t bytes);

	/// frees host pointer
	virtual void free	(void* ptr);

	/// returns the default instance
	static DefaultHostMalloc& getInstance(void);
};

//-------------------------------------------------------------------
	}
}

#endif