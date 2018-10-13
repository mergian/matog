// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/runtime/Malloc.h>
#include <matog/matog.h>
#include <matog/cuda/CUDA.h>

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
DefaultDeviceMalloc DefaultDeviceMalloc::s_instance;
DefaultHostMalloc DefaultHostMalloc::s_instance;

//-------------------------------------------------------------------
void DefaultDeviceMalloc::malloc(CUdeviceptr* ptr, const size_t bytes) {
	cuMemAlloc(ptr, bytes);
	THROWIF(ptr == 0, "OUT_OF_MEMORY");
}

//-------------------------------------------------------------------
void DefaultDeviceMalloc::free(CUdeviceptr ptr) {
	CUresult result = cuMemFree(ptr);
	
	// this is no CRASH error, as the data is freed anyway
	if(result == CUDA_ERROR_DEINITIALIZED)
		return;
		
	CHECK(result);
}

//-------------------------------------------------------------------
void DefaultHostMalloc::malloc(void** ptr, const size_t bytes) {
	*ptr = ::malloc(bytes);
	THROWIF(*ptr == 0, "OUT_OF_MEMORY");
}

//-------------------------------------------------------------------
void DefaultHostMalloc::free(void* ptr) {
	::free(ptr);
}

//-------------------------------------------------------------------
DefaultDeviceMalloc& DefaultDeviceMalloc::getInstance(void) {
	return s_instance;
}

//-------------------------------------------------------------------
DefaultHostMalloc& DefaultHostMalloc::getInstance(void) {
	return s_instance;
}

//-------------------------------------------------------------------
	}
}