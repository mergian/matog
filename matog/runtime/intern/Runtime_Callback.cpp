// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Runtime.h"
#include "exec/Params.h"
#include "exec/Mode.h"
#include "exec/Baseline.h"
#include "exec/Profiler.h"
#include "exec/Optimized.h"

#include <matog/Module.h>

#include <matog/util/String.h>
#include <matog/util/IO.h>

#include <matog/log.h>

#include <vector_functions.h>

using namespace matog::util;

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
void Runtime::CudaInit(unsigned int Flags) {
	CHECK(::cuInit(Flags));	
	
	// init Runtime
	init();
}

//-------------------------------------------------------------------
void Runtime::LaunchKernel(uint32_t kernelId, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, CUstream stream, void **args, void **extra) {
	exec::Params params(
		kernelId, 
		make_uint3(gridDimX, gridDimY, gridDimZ),
		make_uint3(blockDimX, blockDimY, blockDimZ),
		sharedMemBytes,
		stream,
		args,
		extra
	);
	
	switch(s_mode) {
	// DEFAULT
	case exec::Mode::Baseline_SOA:
	case exec::Mode::Baseline_AOS:
	case exec::Mode::Baseline_AOSOA: {
		exec::Baseline baseline(params, s_mode);
		baseline.run();
	} return;

	// PROFILING
	case exec::Mode::Profiling: {
		// get profiler
		exec::Profiler* profiler = exec::Profiler::getProfiler(params);

		if(profiler) {
			profiler->run();

			profiler->postRun();
			FREE(profiler);
		}
	} return;

	// OPTIMIZED
	case exec::Mode::Optimized: {
		exec::Optimized optimized(params);
		optimized.run();
	} return;

	// ERROR CASE
	default:
		;
	}

	THROW("UNKNOWN_APPLICATION_MODE");
}

//-------------------------------------------------------------------
void Runtime::ModuleGetFunction(CUfunction *hfunc, CUmodule hmod, const char *name) {
	uint32_t ptr = (uint32_t)(uint64_t)hmod;
	
	const Module& matogModule = Static::getModule(ptr);

	// get kernel
	const Kernel& kernel = Static::getKernel(name, matogModule.getId());
	*(uint32_t*)hfunc = kernel.getId();
}

//-------------------------------------------------------------------
void Runtime::ModuleLoad(CUmodule *module, const char *fname) {
	if(!(IO::getFileExtension(fname) == "cu")) {
		THROW("non-cu modules are not supported!");
	}

	// load module
	// TODO: replace with const char*?!?
	const String name = IO::getFileName(fname, false);
	
	// get module
	const Module& matogModule = Static::getModule(name, fname);

	// set id 
	*(uint32_t*)module = matogModule.getId();
}

//-------------------------------------------------------------------
void Runtime::ModuleUnload(CUmodule hmod) {
}

//-------------------------------------------------------------------
void Runtime::CtxDestroy(CUcontext ctx) {
	Context::destroy(ctx);
	CHECK(cuCtxDestroy(ctx));
}

//-------------------------------------------------------------------
void Runtime::MemAlloc(CUdeviceptr* ptr, size_t bytes) {
	CHECK(cuMemAlloc(ptr, bytes));
	Context::get().addAllocation(*ptr, bytes);
}

//-------------------------------------------------------------------
void Runtime::MemFree(CUdeviceptr ptr) {
	CHECK(cuMemFree(ptr));
	Context::get().removeAllocation(ptr);
}

//-------------------------------------------------------------------
		}
	}
}