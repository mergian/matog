// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_UTIL_ML_NN
#define __MATOG_UTIL_ML_NN

#include <matog/util/dll.h>
#include <matog/util/ml/Model.h>
#include <vector>

namespace matog {
	namespace util {
		namespace ml {
//---------------------------------------------------------------
class MATOG_UTIL_DLL NN : public Model {
protected:
	struct Item {
		std::vector<float>	input;
		uint32_t			output;
	};

	std::vector<Item>	m_model;
	uint32_t			m_rows;
	uint32_t			m_cols;

	NN(const Type type);
	NN(void);

public:
	virtual ~NN(void);

	virtual uint32_t		predict	(const float* data) const;
	virtual blob			save	(void) const;
	virtual void			load	(const blob model);
	virtual void			train	(const float* input, const int32_t* output, const int cols, const int rows, const uint32_t* counts);

	friend Model* Model::create(const Type);
};

//---------------------------------------------------------------
		}
	}
}

#endif