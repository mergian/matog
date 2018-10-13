// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_UTIL_ML_TYPE
#define __MATOG_UTIL_ML_TYPE

#include "../dll.h"

namespace matog {
	namespace util {
		namespace ml {
			enum class Type {
				NONE = 0,
				SVM_LINEAR,
				SVM_POLY,
				SVM_RBF,
				SVM_SIGMOID,
				SVM_CHI2,
				SVM_INTER,
				NN,
				DOT_NN,
				MULTIPLY_NN
			};
			
			extern Type MATOG_UTIL_DLL getTypeByName(const char* name);
		}
	}
}

#endif