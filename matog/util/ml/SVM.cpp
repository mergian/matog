// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/util/ml/SVM.h>
#include <matog/util/ml/Type.h>
#include <matog/util/IO.h>
#include <matog/log.h>
#include <sstream>
#include <string>

namespace matog {
	namespace util {
		namespace ml {
//-------------------------------------------------------------------
SVM::SVM(const Type type) : Model(type), m_model(new cv::Ptr<cv::ml::SVM>) {
}

//-------------------------------------------------------------------
SVM::~SVM(void) {
	// do not free pointer here! This is a mem leak, but otherwise the cv::Ptr calls a pure virtual function on linux... not sure why
}

//-------------------------------------------------------------------
uint32_t SVM::predict(const float* data) const {
	const int cols = (*m_model)->getVarCount();
	cv::Mat_<float> cvData(1, cols, (float*)data);
	cv::InputArray cvInput(cvData);
	return (uint32_t)(*m_model)->predict(cvInput);
}

//-------------------------------------------------------------------
blob SVM::save(void) const {
	std::ostringstream ss;
	ss << IO::getTmpFolder() << "/matog_svm_tmp.xml";

	const std::string str = ss.str();

	(*m_model)->save(str.c_str());
	blob data = util::IO::readBinary(str.c_str(), true);
	IO::rm(str.c_str());

	return data;
}

//-------------------------------------------------------------------
cv::ml::SVM::KernelTypes SVM::getSVMKernel(void) {
	switch(getType()) {
		case Type::SVM_LINEAR:	return cv::ml::SVM::KernelTypes::LINEAR;
		case Type::SVM_POLY:	return cv::ml::SVM::KernelTypes::POLY;
		case Type::SVM_RBF:		return cv::ml::SVM::KernelTypes::RBF;
		case Type::SVM_SIGMOID:	return cv::ml::SVM::KernelTypes::SIGMOID;
		case Type::SVM_CHI2:	return cv::ml::SVM::KernelTypes::CHI2;
		case Type::SVM_INTER:	return cv::ml::SVM::KernelTypes::INTER;
		default:;
	};

	// should not happen
	THROW();
	return cv::ml::SVM::KernelTypes::LINEAR;
}

//-------------------------------------------------------------------
void SVM::train(const float* input, const int32_t* output, const int cols, const int rows, const uint32_t* counts) {
	cv::Mat_<float>		iMat(rows, cols, (float*)input);
	cv::Mat_<int32_t>	oMat(rows, 1, (int32_t*)output);

	cv::InputArray inputArray(iMat);
	cv::InputArray outputArray(oMat);

	(*m_model) = cv::ml::SVM::create();
	(*m_model)->setKernel(getSVMKernel());
	(*m_model)->train(inputArray, cv::ml::SampleTypes::ROW_SAMPLE, outputArray);
}

//-------------------------------------------------------------------
void SVM::load(const blob model) {
	(*m_model) = cv::Algorithm::loadFromString<cv::ml::SVM>(model.ptr);
}

//-------------------------------------------------------------------
		}
	}
}