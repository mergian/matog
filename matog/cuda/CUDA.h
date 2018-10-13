// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_CUDA_CUDA_H
#define __MATOG_CUDA_CUDA_H

#include <matog/internal.h>
#include <cuda.h>

namespace matog {
	namespace cuda {
//-------------------------------------------------------------------
/// Helper class for easy access to CUDA functions
class MATOG_INTERNAL_DLL CUDA {
	/// prevent initiation
	inline CUDA(void) {}

public:
	/// gets the current device
	static CUdevice			getCurrentDevice		(void);

	/// gets the current context
	static CUcontext		getCurrentContext		(void);
};

//-------------------------------------------------------------------
	}
}
#endif