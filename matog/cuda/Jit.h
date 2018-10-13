// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_CUDA_JIT_H
#define __MATOG_CUDA_JIT_H

#include <matog/util.h>
#include <matog/internal.h>

namespace matog {
	namespace cuda {
//-------------------------------------------------------------------
/// JIT Compiler
/**
	Uses NVCC for Just In Time compilation
*/
class MATOG_INTERNAL_DLL Jit {
private:
	/// locates NVCC. Has to be in $ENV{PATH} variable
	static void locateNVCC(void);

public:
	/// compile a kernel, specified by target. Uses logFile, cubinFile and matogFile as temporary files.
	static const util::blob compile(const jit::Target& target, const char* logFile, const char* cubinFile, const char* matogFile);

	/// generates name for the temporary files
	static void generateFileNames(char*& logFile, char*& cubinFile, char*& matogFile, const char* tmpFolder, const int len);
};
//-------------------------------------------------------------------
	}
}
#endif