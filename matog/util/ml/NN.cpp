// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "NN.h"
#include "Type.h"
#include <matog/util/Mem.h>
#include <matog/log.h>
#include <sstream>
#include <cfloat>
#include <cmath>

namespace matog {
	namespace util {
		namespace ml {
//-------------------------------------------------------------------
NN::NN(const Type type) : Model(type) {
}

//-------------------------------------------------------------------
NN::NN(void) : Model(Type::NN) {
}

//-------------------------------------------------------------------
NN::~NN(void) {
}

//-------------------------------------------------------------------
uint32_t NN::predict(const float* data) const {
	assert(!m_model.empty());

	double bestDist			= DBL_MAX;
	const Item* bestItem	= 0;

	for(const Item& item : m_model) {
		double dist = 0.0;

		uint32_t i = 0;

		for(float value : item.input) {
			dist += std::pow(data[i] - value, 2);
			i++;		
		}

		dist = std::sqrt(dist);
	
		if(dist < bestDist) {
			bestDist = dist;
			bestItem = &item;
		}
	}
	
	L_DEBUG("Dist: %f", bestDist);

	THROWIF(bestItem == 0);

	return bestItem->output;
}

//-------------------------------------------------------------------
blob NN::save(void) const {
	const size_t size = 2 * sizeof(uint32_t) + sizeof(float) * m_cols * m_rows + sizeof(uint32_t) * m_rows;

	uint32_t* ptr = (uint32_t*)NEW_A(char, size);
	ptr[0] = m_rows;
	ptr[1] = m_cols;

	uint32_t i = 2;

	for(const auto& item : m_model) {
		ptr[i] = item.output;
		memcpy(&ptr[i+1], &item.input[0], sizeof(float) * m_cols);

		i += m_cols + 1;
	}

	return blob(ptr, size);
}

//-------------------------------------------------------------------
void NN::train(const float* input, const int32_t* output, const int cols, const int rows, const uint32_t* counts) {
	m_rows = rows;
	m_cols = cols;

	m_model.clear();
	m_model.reserve(m_rows);

	for(uint32_t row = 0; row < m_rows; row++) {
		// create new
		m_model.emplace_back();

		// get item and set values
		Item& item = m_model.back();
		item.output = output[row];
		item.input.resize(m_cols);

		// set col values
		for(uint32_t col = 0; col < m_cols; col++) {
			item.input[col] = input[col + row * cols];
		}
	}
}

//-------------------------------------------------------------------
void NN::load(const blob model) {
	uint32_t* ptr = (uint32_t*)model.ptr;

	// load header data
	m_rows = ptr[0];
	m_cols = ptr[1];
	
	// clear and resize
	m_model.clear();
	m_model.resize(m_rows);

	// increment ptr
	ptr += 2;

	for(uint32_t row = 0; row < m_rows; row++) {
		auto& item = m_model[row];

		item.output = ptr[0];
		item.input.resize(m_cols);
		memcpy(&item.input[0], &ptr[1], sizeof(float) * m_cols);
		
		ptr += m_cols + 1;
	}
}

//-------------------------------------------------------------------
		}
	}
}