// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CUDA_ARCHITECTURE
#define __MATOG_CUDA_ARCHITECTURE

namespace matog {
	namespace cuda {
//-------------------------------------------------------------------
/// GPU architectures
enum class Architecture {
	UNKNOWN		= 0,
	FERMI		= 20,
	KEPLER_30	= 30,
	KEPLER_35	= 35,
	MAXWELL_50	= 50,
	MAXWELL_52	= 52,
	MAXWELL_53  = 53,
	PASCAL_60	= 60,
	PASCAL_61	= 61,

	// SPECIAL VALUES
	MIN_ARCH	= FERMI,
	MAX_ARCH	= PASCAL_61
};

//-------------------------------------------------------------------
	}
}

#endif