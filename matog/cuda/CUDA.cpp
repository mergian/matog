// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/cuda/CUDA.h>
#include <matog/log.h>

namespace matog {
	namespace cuda {

//-------------------------------------------------------------------
CUcontext CUDA::getCurrentContext(void) {
	CUcontext context;
	CHECK(cuCtxGetCurrent(&context));
	return context;
}

//-------------------------------------------------------------------
CUdevice CUDA::getCurrentDevice(void) {
	CUdevice device;
	CHECK(cuCtxGetDevice(&device));
	return device;
}

//-------------------------------------------------------------------
	}
}