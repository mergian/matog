// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "DotNN.h"
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
DotNN::DotNN(void) : NN(Type::DOT_NN) {
}

//-------------------------------------------------------------------
DotNN::~DotNN(void) {
}

//-------------------------------------------------------------------
uint32_t DotNN::predict(const float* data) const {
	double bestAngle		= -DBL_MAX;
	const Item* bestItem	= &m_model.front();
	
	for(const Item& item : m_model) {
		double scalar  = 0.0;
		double lengthA = 0.0;
		double lengthB = 0.0;

		// calc scalar product and length(data) length(item.input)
		uint32_t i = 0;
		for(float value : item.input) {
			scalar	+= data[i] * value;
			lengthA += data[i] * data[i];
			lengthB += value * value;
			i++;
		}
	
		lengthA = std::sqrt(lengthA);
		lengthB = std::sqrt(lengthB);
		
		double angle = scalar / (lengthA * lengthB);
		
		// ensure it is in range -1.0 to 1.0
		if(angle > 1.0)			angle = 1.0;
		else if(angle < -1.0)	angle = -1.0;
		
		// we look for the 1.0, as this means, that the vector look into the same direction
		if(angle > bestAngle) {
			bestAngle = angle;
			bestItem  = &item;
		}
	}
	
	THROWIF(bestItem == 0);

	return bestItem->output;
}

//-------------------------------------------------------------------
		}
	}
}