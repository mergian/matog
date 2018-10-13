// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_PREDICTOR_META
#define __MATOG_RUNTIME_INTERN_PREDICTOR_META

#include "Item.h"
#include <matog/util.h>
#include <matog/runtime.h>
#include <list>
#include <vector>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace predictor {
//-------------------------------------------------------------------
class Meta {
protected:
	std::list<Item>		m_meta;
	util::ml::Model*	m_model;

public:
	Meta(util::ml::Model* model);
	~Meta(void);

	inline util::ml::Model* getModel(void) const { return m_model; }
	void getData(std::vector<float>& data, const exec::Params* params) const;
	void addMeta(const uint32_t decisionId, const uint32_t offset, const uint32_t dim);
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif