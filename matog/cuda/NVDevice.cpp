// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <cuda.h>
#include <matog/cuda/NVDevice.h>
#include <matog/cuda/Architecture.h>
#include <matog/util/Mem.h>
#include <matog/log.h>

#if WIN32
#pragma warning(disable: 4996)
#endif

namespace matog {
	namespace cuda {
//-------------------------------------------------------------------
const char* NVDevice::getName(const CUdevice device) {
	char name[128];
	CHECK(cuDeviceGetName(name, sizeof(name), device));
	return NEW_STRING(name);
}

//-------------------------------------------------------------------
uint32_t NVDevice::getCC(const CUdevice device) {
	int major = 0, minor = 0;
	CHECK(cuDeviceGetAttribute(&major, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, device)); 
	CHECK(cuDeviceGetAttribute(&minor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, device)); 
	uint32_t cc = (uint32_t)(major * 10 + minor);
	
	// it seems as if CC21 is not valid for the compiler... not sure why
	if(cc == 21)
		cc = 20;
	
	// 61 is invalid for compiler
	//if(cc == 61)
	//	cc = 60;
	
	return cc;
}

//-------------------------------------------------------------------
uint64_t NVDevice::getGlobalMem(const CUdevice device) {
	size_t globalMem;
	CHECK(cuDeviceTotalMem(&globalMem, device));
	return (uint64_t)globalMem;
}

//-------------------------------------------------------------------
uint32_t NVDevice::getSharedMem(const CUdevice device) {
	int sharedMem;
	CHECK(cuDeviceGetAttribute(&sharedMem, CU_DEVICE_ATTRIBUTE_SHARED_MEMORY_PER_BLOCK, device));
	return sharedMem;
}

//-------------------------------------------------------------------
uint32_t NVDevice::getConstMem(const CUdevice device) {
	int constMem;
	CHECK(cuDeviceGetAttribute(&constMem, CU_DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY, device));
	return constMem;
}

//-------------------------------------------------------------------
uint32_t NVDevice::getSMCount(const CUdevice device) {
	int smCount;
	CHECK(cuDeviceGetAttribute(&smCount, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, device));
	return smCount;
}

//-------------------------------------------------------------------
uint32_t NVDevice::getMaxThreadsPerBlock(const CUdevice device) {
	int theadsPerBlock;
	CHECK(cuDeviceGetAttribute(&theadsPerBlock, CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK, device));
	return theadsPerBlock;
}

//-------------------------------------------------------------------
uint32_t NVDevice::getMaxThreadsPerSM(const CUdevice device) {
	int threadsPerSM;
	CHECK(cuDeviceGetAttribute(&threadsPerSM, CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR, device));
	return threadsPerSM;
}

//-------------------------------------------------------------------
uint32_t NVDevice::getMaxTextureWidth(const CUdevice device) {
	int texWidth;
	CHECK(cuDeviceGetAttribute(&texWidth, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LINEAR_WIDTH, device));
	return texWidth;
}

//-------------------------------------------------------------------
uint32_t NVDevice::getTextureAlignment(const CUdevice device) {
	int texAlignment;
	CHECK(cuDeviceGetAttribute(&texAlignment, CU_DEVICE_ATTRIBUTE_TEXTURE_ALIGNMENT, device));
	return texAlignment;
}

//-------------------------------------------------------------------
Architecture NVDevice::getArchitecture(const CUdevice device) {
	const uint32_t cc = getCC(device);
	if(cc >= 20 && cc < 30) 
		return Architecture::FERMI;
	else if(cc >= 30 && cc < 35)
		return Architecture::KEPLER_30;
	else if(cc >= 35 && cc < 50)
		return Architecture::KEPLER_35;
	else if(cc >= 50 && cc < 52)
		return Architecture::MAXWELL_50;
	else if(cc >= 52)
		return Architecture::MAXWELL_52;
	else if(cc == 60)
		return Architecture::PASCAL_60;
	else if(cc == 61)
		return Architecture::PASCAL_61;

	return Architecture::UNKNOWN;
}

//-------------------------------------------------------------------
	}
}