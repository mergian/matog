// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_UTIL_ML_SVM
#define __MATOG_UTIL_ML_SVM

#include <matog/util/dll.h>
#include <matog/util/ml/Model.h>
#include <opencv2/ml.hpp>

namespace matog {
	namespace util {
		namespace ml {
//-------------------------------------------------------------------
class MATOG_UTIL_DLL SVM : public Model {
	cv::Ptr<cv::ml::SVM>* m_model;

	cv::ml::SVM::KernelTypes getSVMKernel(void);
	SVM(const Type type);

public:
	virtual ~SVM(void);

	virtual uint32_t		predict	(const float* data) const;
	virtual blob			save	(void) const;
	virtual void			load	(const blob model);
	virtual void			train	(const float* input, const int32_t* output, const int cols, const int rows, const uint32_t* counts);

	friend Model* Model::create(const Type);
};

//-------------------------------------------------------------------
		}
	}
}


#endif