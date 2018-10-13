// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/matog.h>
#include <matog/runtime/intern/Runtime.h>
#include <matog/runtime/DeviceArray.h>
#include <matog/runtime/HostArray.h>
#include <matog/Static.h>

using namespace matog::runtime;
using matog::runtime::intern::Runtime;
using matog::Static;

#define TRY_CUDA(...) __VA_ARGS__; return CUDA_SUCCESS

namespace matog {
//-------------------------------------------------------------------
float MATOG_RUNTIME_DLL getVersion(void) {
	return Static::getVersion();
}

//-------------------------------------------------------------------
void MATOG_RUNTIME_DLL registerAllocation(CUdeviceptr ptr, const uint64_t bytes) {
	Runtime::registerAllocation(ptr, bytes);
}

//-------------------------------------------------------------------
void MATOG_RUNTIME_DLL deregisterAllocation(CUdeviceptr ptr) {
	Runtime::deregisterAllocation(ptr);
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL Init(unsigned int Flags) {
	TRY_CUDA(Runtime::CudaInit(Flags));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL CtxDestroy(CUcontext ctx) {
	TRY_CUDA(Runtime::CtxDestroy(ctx));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL LaunchKernel(CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, CUstream stream, void **args, void **extra) {
	TRY_CUDA(Runtime::LaunchKernel((uint32_t)(uint64_t)f, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, stream, args, extra));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL ModuleGetFunction(CUfunction *hfunc, CUmodule hmod, const char *name) {
	TRY_CUDA(Runtime::ModuleGetFunction(hfunc, hmod, name));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL ModuleLoad(CUmodule *module, const char *fname) {
	TRY_CUDA(Runtime::ModuleLoad(module, fname));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL ModuleUnload(CUmodule hmod) {
	TRY_CUDA(Runtime::ModuleUnload(hmod));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL MemAlloc(CUdeviceptr* ptr, size_t bytes) {
	TRY_CUDA(Runtime::MemAlloc(ptr, bytes));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL MemFree(CUdeviceptr ptr) {
	TRY_CUDA(Runtime::MemFree(ptr));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL setCompilerFlags(CUfunction f, const char* flags) {
	TRY_CUDA(Runtime::setCompilerFlags(f, flags));
}

//-------------------------------------------------------------------
void MATOG_RUNTIME_DLL watch(const int8_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
void MATOG_RUNTIME_DLL watch(const int16_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
void MATOG_RUNTIME_DLL watch(const int32_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
void MATOG_RUNTIME_DLL watch(const int64_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
void MATOG_RUNTIME_DLL watch(const uint8_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
void MATOG_RUNTIME_DLL watch(const uint16_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
void MATOG_RUNTIME_DLL watch(const uint32_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
void MATOG_RUNTIME_DLL watch(const uint64_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
void MATOG_RUNTIME_DLL watch(const float* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
void MATOG_RUNTIME_DLL watch(const double* ptr, const char* file, const int line)	{ Runtime::watch(ptr, file, line); }
	
//-------------------------------------------------------------------
void MATOG_RUNTIME_DLL unwatch(const void* ptr) {
	Runtime::unwatch(ptr);
}

//-------------------------------------------------------------------
}


//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL cuMemcpy(DeviceArray* const dst, const DeviceArray* const src, const size_t bytes) {
	TRY_CUDA(Runtime::Memcpy((intern::DeviceArray*)dst, (intern::DeviceArray*)src));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL cuMemcpyAsync(DeviceArray* const dst, const DeviceArray* const src, const size_t bytes, CUstream hStream) {
	TRY_CUDA(Runtime::MemcpyAsync((intern::DeviceArray*)dst, (intern::DeviceArray*)src, hStream));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL cuMemcpyDtoD(DeviceArray* const dstDevice, const DeviceArray* const srcDevice, const size_t bytes) {
	TRY_CUDA(Runtime::MemcpyDtoD((intern::DeviceArray*)dstDevice, (intern::DeviceArray*)srcDevice));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL cuMemcpyDtoDAsync(DeviceArray* const dstDevice, const DeviceArray* const srcDevice, const size_t bytes, CUstream hStream) {
	TRY_CUDA(Runtime::MemcpyDtoDAsync((intern::DeviceArray*)dstDevice, (intern::DeviceArray*)srcDevice, hStream));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL cuMemcpyDtoH(HostArray* const dstHost, const DeviceArray* const srcDevice, const size_t bytes) {
	TRY_CUDA(Runtime::MemcpyDtoH((intern::HostArray*)dstHost, (intern::DeviceArray*)srcDevice));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL cuMemcpyDtoHAsync(HostArray* const dstHost, const DeviceArray* const srcDevice, const size_t bytes, CUstream hStream) {
	TRY_CUDA(Runtime::MemcpyDtoHAsync((intern::HostArray*)dstHost, (intern::DeviceArray*)srcDevice, hStream));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL cuMemcpyHtoDAsync(DeviceArray* const dstDevice, const HostArray* const srcHost, const size_t bytes, CUstream hStream) {
	TRY_CUDA(Runtime::MemcpyHtoDAsync((intern::DeviceArray*)dstDevice, (intern::HostArray*)srcHost, hStream));
}

//-------------------------------------------------------------------
CUresult MATOG_RUNTIME_DLL cuMemcpyHtoD(DeviceArray* const dstDevice, const HostArray* const srcHost, const size_t bytes) {
	TRY_CUDA(Runtime::MemcpyHtoD((intern::DeviceArray*)dstDevice, (intern::HostArray*)srcHost));
}