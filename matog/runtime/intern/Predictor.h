// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_RUNTIME_INTERN_PREDICTOR_H
#define __MATOG_RUNTIME_INTERN_PREDICTOR_H

#include <matog/runtime.h>
#include "predictor/Meta.h"

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
class Predictor {
private:
	typedef std::map<uint32_t, predictor::Meta>	Models;

	static Models s_models;

	Predictor(void) = delete;

public:
	static uint32_t		predict	(const uint32_t decisionId, const exec::Params* params = 0);
	static bool			init	(void);
};

//-------------------------------------------------------------------
		}
	}
}
#endif