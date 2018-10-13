// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_RUNTIME_INTERN_CONTROLLER
#define __MATOG_RUNTIME_INTERN_CONTROLLER

#include <matog/runtime.h>
#include "Context.h"
#include <vector>

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
/// Main controlling interface
/**
	Initiates MATOG and controlls all communication from
	the MATOG lib to the application.

	It reacts to the usage of ENV vars.

	ENV{MATOG_CLEAR_CUBINS=1} = removes all cubins from the DB
	ENV{MATOG_CLEAR_DB=1} = factory resets the entire DB
	ENV{MATOG_MODE=1} = enables profiling
*/
class Runtime {
private:
	/// RunMode
	static exec::Mode	s_mode;			///< stores the mode

	/// gets an config for a specific MATOG array
	static matog::variant::Hash	getOptimalConfig			(const BaseArray<>* const ptr);

	/// Registers user defined meta data
	static void					watch						(const void* ptr, const array::field::Type type, const char* file, const int line);

public:
	/// initiates the Controller
	static void					init						(void);

	/// Registers user defined meta data
	static void					watch						(const int8_t* ptr, const char* file, const int line);
	static void					watch						(const int16_t* ptr, const char* file, const int line);
	static void					watch						(const int32_t* ptr, const char* file, const int line);
	static void					watch						(const int64_t* ptr, const char* file, const int line);
	static void					watch						(const uint8_t* ptr, const char* file, const int line);
	static void					watch						(const uint16_t* ptr, const char* file, const int line);
	static void					watch						(const uint32_t* ptr, const char* file, const int line);
	static void					watch						(const uint64_t* ptr, const char* file, const int line);
	static void					watch						(const float* ptr, const char* file, const int line);
	static void					watch						(const double* ptr, const char* file, const int line);
	
	/// Deregisters user defined meta data
	static void					unwatch						(const void* ptr);

	/// registers a MATOG array
	static void					registerArray				(BaseArray<>* const ptr, const uint32_t arrayId, const bool isDevice);

	/// deregisters a MATOG array
	static void					deregisterArray				(BaseArray<>* const ptr);

	/// registers a CUDA allocation
	static void					registerAllocation			(CUdeviceptr ptr, const uint64_t bytes);

	/// deregisters a CUDA allocation
	static void					deregisterAllocation		(CUdeviceptr ptr);
						
	/// Callback: cuInit
	static void					CudaInit					(unsigned int Flags);

	/// Callback: cuLaunchKernel
	static void					LaunchKernel				(uint32_t kernelId, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, CUstream hStream, void **args, void **extra);
			
	/// Callback: cuMemAlloc
	static void					MemAlloc					(CUdeviceptr* ptr, size_t bytes);

	/// Callback: cuMemFree
	static void					MemFree						(CUdeviceptr ptr);
	
	/// Callback: cuMemcpy
	static void					Memcpy						(DeviceArray* const dest, const DeviceArray* const src);
	
	/// Callback: cuMemcypAsync
	static void					MemcpyAsync					(DeviceArray* const dest, const DeviceArray* const src, CUstream hStream);

	/// Callback: cuMemcpyDtoD
	static void					MemcpyDtoD					(DeviceArray* const dest, const DeviceArray* const src);

	/// Callback: cuMemcpyDtoDAsync
	static void					MemcpyDtoDAsync				(DeviceArray* const dest, const DeviceArray* const src, CUstream hStream);

	/// Callback: cuMemcpyDtoH
	static void					MemcpyDtoH					(HostArray* const dest, const DeviceArray* const src);

	/// Callback: cuMemcpyDtoHAsync
	static void					MemcpyDtoHAsync				(HostArray* const dest, const DeviceArray* const src, CUstream hStream);

	/// Callback: cuMemcpyHtoD
	static void					MemcpyHtoD					(DeviceArray* const dest, const HostArray* const src);

	/// Callback: cuMemcpyHtoDAsync
	static void					MemcpyHtoDAsync				(DeviceArray* const dest, const HostArray* const src, CUstream hStream);

	/// Callback: cuModuleGetFunction. hmod contains the ID of the module. hfunc will be used to store the ID of the kernel.
	static void					ModuleGetFunction			(CUfunction *hfunc, CUmodule hmod, const char *name);
	
	/// Callback: cuModuleLoad. module will be used to store the ID of the module. fname has to point to a source file.
	static void					ModuleLoad					(CUmodule *module, const char *fname);

	/// Callback: cuModuleUnload
	static void					ModuleUnload				(CUmodule hmod);

	/// Callback: cuCtxDestroy
	static void					CtxDestroy					(CUcontext ctx);

	/// sets the compiler flags for a specific kernel. CUfunction will be cast to int and contains the ID of the kernel.
	static void					setCompilerFlags			(CUfunction f, const char* flags);

	/// is this a profiling run?
	static bool					isProfiling					(void);

	/// is this an optimized run?
	static bool					isOptimized					(void);
	
	/// is this a baseline run?
	static bool					isBaseline					(void);
};

//-------------------------------------------------------------------
		}
	}
}
#endif