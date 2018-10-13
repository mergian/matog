// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_PARAMS
#define __MATOG_RUNTIME_INTERN_EXEC_PARAMS

#include <vector_types.h>
#include <cstdint>
#include <cuda.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
struct Params {
		  uint32_t decisionId;
	const uint32_t kernelId;
	const uint3 grid;
	const uint3 block;
	const uint32_t sharedMemBytes;
	const CUstream stream;
	void** args;
	void** extra;

	inline Params(const uint32_t _kernelId, const uint3 _grid, const uint3 _block, const uint32_t _sharedMemBytes, const CUstream _stream, void** _args, void** _extra) :
		decisionId(0),
		kernelId(_kernelId),
		grid(_grid),
		block(_block),
		sharedMemBytes(_sharedMemBytes),
		stream(_stream),
		args(_args),
		extra(_extra)
	{}
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif