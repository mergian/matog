// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CUDA_NVDEVICE
#define __MATOG_CUDA_NVDEVICE

#include <cuda.h>
#include <cstdint>
#include <matog/internal.h>

namespace matog {
	namespace cuda {
//-------------------------------------------------------------------
/// Helper Class for easy access of CUDA device related functions
class MATOG_INTERNAL_DLL NVDevice {
	/// prevent initiation
	inline NVDevice(void) {}
	
public:
	static const char*		getName					(const CUdevice device);	///< gets name
	static uint32_t			getCC					(const CUdevice device);	///< gets compute capability
	static uint64_t			getGlobalMem			(const CUdevice device);	///< gets total global mem
	static uint32_t			getSharedMem			(const CUdevice device);	///< gets total shared mem
	static uint32_t			getConstMem				(const CUdevice device);	///< gets total const mem
	static uint32_t			getSMCount				(const CUdevice device);	///< gets count of multiprocessors
	static uint32_t			getMaxThreadsPerBlock	(const CUdevice device);	///< gets max threads per block
	static uint32_t			getMaxThreadsPerSM		(const CUdevice device);	///< gets max threads per multiprocessor
	static Architecture		getArchitecture			(const CUdevice device);	///< gets Architecture
	static uint32_t			getMaxTextureWidth		(const CUdevice device);	///< gets max texture width
	static uint32_t			getTextureAlignment		(const CUdevice device);	///< gets necessary texture alignment constraint
};

//-------------------------------------------------------------------
	}
}

#endif