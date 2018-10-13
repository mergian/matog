// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef MATOG
#define MATOG

#include <matog/runtime/dll.h>
#include <matog/runtime/Malloc.h>
#include <matog/runtime/BaseArray.h>
#include <matog/runtime/DynArray.h>
#include <matog/runtime/DeviceArray.h>
#include <matog/runtime/HostArray.h>
#include <matog/log.h>
#include <matog/enums.h>

namespace matog {
	/// Registers user defined meta data
	extern void MATOG_RUNTIME_DLL watch(const int8_t* ptr, const char* file, const int line);
	extern void MATOG_RUNTIME_DLL watch(const int16_t* ptr, const char* file, const int line);
	extern void MATOG_RUNTIME_DLL watch(const int32_t* ptr, const char* file, const int line);
	extern void MATOG_RUNTIME_DLL watch(const int64_t* ptr, const char* file, const int line);
	extern void MATOG_RUNTIME_DLL watch(const uint8_t* ptr, const char* file, const int line);
	extern void MATOG_RUNTIME_DLL watch(const uint16_t* ptr, const char* file, const int line);
	extern void MATOG_RUNTIME_DLL watch(const uint32_t* ptr, const char* file, const int line);
	extern void MATOG_RUNTIME_DLL watch(const uint64_t* ptr, const char* file, const int line);
	extern void MATOG_RUNTIME_DLL watch(const float* ptr, const char* file, const int line);
	extern void MATOG_RUNTIME_DLL watch(const double* ptr, const char* file, const int line);
	
	/// Deregisters user defined meta data
	extern void MATOG_RUNTIME_DLL unwatch(const void* ptr);

	/// Registers a CUdeviceptr to restore it during the profiling
	extern void MATOG_RUNTIME_DLL registerAllocation(CUdeviceptr ptr, const uint64_t bytes);    

	/// Unregisters a CUdeviceptr
	extern void MATOG_RUNTIME_DLL deregisterAllocation(CUdeviceptr ptr);

	/// cuInit
	extern CUresult MATOG_RUNTIME_DLL Init(unsigned int Flags);

	/// cuLaunchKernel
	extern CUresult MATOG_RUNTIME_DLL LaunchKernel(CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, CUstream hStream, void **kernelParams, void **extra);

	/// cuModuleGetFunction
	extern CUresult MATOG_RUNTIME_DLL ModuleGetFunction(CUfunction *hfunc, CUmodule hmod, const char *name);

	/// cuModuleLoad
	extern CUresult MATOG_RUNTIME_DLL ModuleLoad(CUmodule *module, const char *fname);

	/// cuModuleUnlod
	extern CUresult MATOG_RUNTIME_DLL ModuleUnload(CUmodule hmod);

	/// cuCtxDestroy
	extern CUresult MATOG_RUNTIME_DLL CtxDestroy(CUcontext ctx);

	/// cuMemAlloc
	extern CUresult MATOG_RUNTIME_DLL MemAlloc(CUdeviceptr* ptr, size_t bytes);

	/// cuMemFree
	extern CUresult MATOG_RUNTIME_DLL MemFree(CUdeviceptr ptr);

	/// sets compiler flags for a given CUfunction
	extern CUresult MATOG_RUNTIME_DLL setCompilerFlags(CUfunction f, const char* flags);

	/// gets the current MATOG Version
	extern float MATOG_RUNTIME_DLL getVersion(void);
}

/// cuMemcpy
extern CUresult MATOG_RUNTIME_DLL cuMemcpy(matog::runtime::DeviceArray* const dst, const matog::runtime::DeviceArray* const src, const size_t bytes);

/// cuMemcpyAsync
extern CUresult MATOG_RUNTIME_DLL cuMemcpyAsync(matog::runtime::DeviceArray* const dst, const matog::runtime::DeviceArray* const src, const size_t bytes, CUstream hStream);

/// cuMemcpyDtoD
extern CUresult MATOG_RUNTIME_DLL cuMemcpyDtoD(matog::runtime::DeviceArray* const dstDevice, const matog::runtime::DeviceArray* const srcDevice, const size_t bytes);

/// cuMemcpyDtoH
extern CUresult MATOG_RUNTIME_DLL cuMemcpyDtoDAsync(matog::runtime::DeviceArray* const dstDevice, const matog::runtime::DeviceArray* const srcDevice, const size_t bytes, CUstream hStream);

/// cuMemcpyDtoH
extern CUresult MATOG_RUNTIME_DLL cuMemcpyDtoH(matog::runtime::HostArray* const dstHost, const matog::runtime::DeviceArray* const srcDevice, const size_t bytes);

/// cuMemcpyDtoHAsync
extern CUresult MATOG_RUNTIME_DLL cuMemcpyDtoHAsync(matog::runtime::HostArray* const dstHost, const matog::runtime::DeviceArray* const srcDevice, const size_t bytes, CUstream hStream);

/// cuMemcpyHtoD
extern CUresult MATOG_RUNTIME_DLL cuMemcpyHtoD(matog::runtime::DeviceArray* const dstDevice, const matog::runtime::HostArray* const srcHost, const size_t bytes);

/// cuMemcpyHtoDAsync
extern CUresult MATOG_RUNTIME_DLL cuMemcpyHtoDAsync(matog::runtime::DeviceArray* const dstDevice, const matog::runtime::HostArray* const srcHost, const size_t bytes, CUstream hStream);

//-------------------------------------------------------------------
// define new matog defines
#undef cuCtxDestroy
#undef cuMemAlloc
#undef cuMemFree
#define cuInit				matog::Init
#define cuLaunchKernel		matog::LaunchKernel
#define cuModuleGetFunction	matog::ModuleGetFunction
#define cuModuleLoad		matog::ModuleLoad
#define cuModuleUnload		matog::ModuleUnload
#define cuCtxDestroy		matog::CtxDestroy
#define cuMemAlloc			matog::MemAlloc
#define cuMemFree			matog::MemFree

#endif