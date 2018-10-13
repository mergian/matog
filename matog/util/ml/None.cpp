// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/util/ml/None.h>
#include <matog/util/ml/Type.h>
#include <matog/util/Mem.h>
#include <matog/log.h>
#include <sstream>
#include <map>

namespace matog {
	namespace util {
		namespace ml {
//-------------------------------------------------------------------
None::None(void) : Model(Type::NONE), m_value(0) {
}

//-------------------------------------------------------------------
None::~None(void) {
}

//-------------------------------------------------------------------
uint32_t None::predict(const float*) const {
	return m_value;
}

//-------------------------------------------------------------------
blob None::save(void) const {
	uint32_t* ptr = NEW_A(uint32_t, 1);
	ptr[0] = m_value;
	return blob(ptr, sizeof(uint32_t));
}

//-------------------------------------------------------------------
void None::train(const float* input, const int32_t* output, const int cols, const int rows, const uint32_t* counts) {
	std::map<int32_t, uint32_t> tmp;
	
	for(int i = 0; i < rows; i++) {
		tmp[output[i]] += counts[i];
	}
	
	int maxValue = 0;
	uint32_t maxCount = 0;
	
	for(const auto& it : tmp) {
		if(it.second > maxCount || (it.second == maxCount && it.first < maxValue)) {
			maxValue = it.first;
			maxCount = it.second;
		}
	}
	
	m_value = maxValue;
}

//-------------------------------------------------------------------
void None::load(const blob model) {
	assert(model.size == sizeof(uint32_t));
	m_value = ((uint32_t*)model.ptr)[0];
}

//-------------------------------------------------------------------
		}
	}
}