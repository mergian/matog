// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/util/ml/Model.h>
#include <matog/util/ml/None.h>
#include <matog/util/ml/SVM.h>
#include <matog/util/ml/NN.h>
#include <matog/util/ml/DotNN.h>
#include <matog/util/ml/MultiplyNN.h>
#include <matog/util/ml/Type.h>
#include <matog/util/Mem.h>
#include <matog/log.h>

namespace matog {
	namespace util {
		namespace ml {
//-------------------------------------------------------------------
Model::Model(const Type type) : m_type(type) {
}

//-------------------------------------------------------------------
Model::~Model(void) {
}

//-------------------------------------------------------------------
Model* Model::create(const Type type) {
	if(type == Type::NONE)			return NEW(None);
	if(type == Type::SVM_LINEAR)	return NEW(SVM(Type::SVM_LINEAR));
	if(type == Type::SVM_POLY)		return NEW(SVM(Type::SVM_POLY));
	if(type == Type::SVM_RBF)		return NEW(SVM(Type::SVM_RBF));
	if(type == Type::SVM_CHI2)		return NEW(SVM(Type::SVM_CHI2));
	if(type == Type::SVM_SIGMOID)	return NEW(SVM(Type::SVM_SIGMOID));
	if(type == Type::SVM_INTER)		return NEW(SVM(Type::SVM_INTER));
	if(type == Type::NN)			return NEW(NN);
	if(type == Type::DOT_NN)		return NEW(DotNN);
	if(type == Type::MULTIPLY_NN)	return NEW(MultiplyNN);
	THROW("Unknown ML type");
	return 0;
}

//-------------------------------------------------------------------
		}
	}
}