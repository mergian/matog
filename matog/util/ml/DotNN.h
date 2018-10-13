// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_UTIL_ML_DOTNN
#define __MATOG_UTIL_ML_DOTNN

#include <matog/util/ml/NN.h>

namespace matog {
	namespace util {
		namespace ml {
//---------------------------------------------------------------
class MATOG_UTIL_DLL DotNN : public NN {
	DotNN(void);

public:
	virtual ~DotNN(void);
	virtual uint32_t predict(const float* data) const;
	friend Model* Model::create(const Type);
};

//-------------------------------------------------------------------
		}
	}
}

#endif