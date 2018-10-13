// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "MultiplyNN.h"
#include <matog/util/ml/Type.h>
#include <matog/util/Mem.h>
#include <matog/log.h>
#include <sstream>
#include <string>
#include <cfloat>
#include <cmath>

namespace matog {
	namespace util {
		namespace ml {
//-------------------------------------------------------------------
MultiplyNN::MultiplyNN(void) : Model(Type::MULTIPLY_NN), m_rows(0), m_cols(0) {
}

//-------------------------------------------------------------------
MultiplyNN::~MultiplyNN(void) {
}

//-------------------------------------------------------------------
uint32_t MultiplyNN::predict(const float* data) const {
	assert(m_cols != 0);
	assert(m_rows != 0);
	assert(!m_model.empty());

	// prepare input
	double input = 0.0;

	for(size_t i = 0; i < m_cols; i++) {
		const double tmp = (double)data[i];
		if(tmp != 0.0) input *= tmp;
	}

	// init
	double		bestDist  = DBL_MAX;
	uint32_t	bestValue = 0;

	// iterate all items
	for(const Item& item : m_model) {
		const double dist = std::abs(input - item.input);

		if(dist < bestDist) {
			bestDist  = dist;
			bestValue = item.output;
		}
	}
	
	assert(bestDist != DBL_MAX);

	return bestValue;
}

//-------------------------------------------------------------------
void MultiplyNN::train(const float* input, const int32_t* output, const int cols, const int rows, const uint32_t* counts) {
	m_model.clear();
	m_model.reserve(rows);
	m_cols = cols;
	m_rows = rows;

	for(uint32_t row = 0; row < m_rows; row++) {
		double value = 0.0;
		
		for(uint32_t col = 0; col < m_cols; col++) {
			const double tmp = input[col + row * cols];
			if(tmp != 0.0) value *= tmp;
		}
		
		m_model.emplace_back(output[row], value);
	}
}

//-------------------------------------------------------------------
blob MultiplyNN::save(void) const {
	const size_t size = 2 * sizeof(uint32_t) + sizeof(Item) * m_model.size();

	// set counts
	uint32_t* ptr = (uint32_t*)NEW_A(char, size);
	ptr[0] = m_rows;
	ptr[1] = m_cols;

	// copy items
	memcpy(&ptr[2], &m_model[0], sizeof(Item) * m_model.size());

	return blob(ptr, size);
}

//-------------------------------------------------------------------
void MultiplyNN::load(const blob model) {
	const uint32_t* ptr = (uint32_t*)model.ptr;

	// get header
	m_rows = ptr[0];
	m_cols = ptr[1];

	// clear and resize
	m_model.clear();
	m_model.resize(m_rows);

	// copy items
	memcpy(&m_model[0], &ptr[2], model.size - sizeof(uint32_t) * 2);
}

//-------------------------------------------------------------------
		}
	}
}