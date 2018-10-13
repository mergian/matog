// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/util/ml/Type.h>
#include <matog/log.h>
#include <cstring>

namespace matog {
	namespace util {
		namespace ml {
//-------------------------------------------------------------------
extern Type MATOG_UTIL_DLL getTypeByName(const char* name) {
	if(strcmp(name, "multiply_nn") == 0)	return Type::MULTIPLY_NN;
	if(strcmp(name, "dot_nn") == 0)			return Type::DOT_NN;
	if(strcmp(name, "nn") == 0)				return Type::NN;
	if(strcmp(name, "none") == 0)			return Type::NONE;
	if(strcmp(name, "svm_chi2") == 0)		return Type::SVM_CHI2;
	if(strcmp(name, "svm_inter") == 0)		return Type::SVM_INTER;
	if(strcmp(name, "svm_linear") == 0)		return Type::SVM_LINEAR;
	if(strcmp(name, "svm_poly") == 0)		return Type::SVM_POLY;
	if(strcmp(name, "svm_rbf") == 0)		return Type::SVM_RBF;
	if(strcmp(name, "svm_sigmoid") == 0)	return Type::SVM_SIGMOID;

	THROW("unknown model type. supported values: direction, euclidean, none, svm_chi2, svm_inter, svm_linear, svm_poly, svm_rbf or svm_sigmoid.");
	return Type::NONE;
}

//-------------------------------------------------------------------
		}
	}
}