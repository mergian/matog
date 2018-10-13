// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Meta.h"
#include "Item.h"
#include <matog/runtime/intern/exec/Params.h>
#include <matog/runtime/intern/Context.h>

#include <matog/Static.h>

#include <matog/util/Mem.h>
#include <matog/util/ml/Model.h>

#include <matog/log.h>

using namespace matog::util::ml;

namespace matog {
	namespace runtime {
		namespace intern {
			namespace predictor {
//-------------------------------------------------------------------
Meta::Meta(Model* model) : m_model(model) {
}

//-------------------------------------------------------------------
Meta::~Meta(void) {
	if(m_model)
		FREE(m_model);
}

//-------------------------------------------------------------------
void Meta::addMeta(const uint32_t decisionId, const uint32_t offset, const uint32_t dim) {
	m_meta.emplace_back(decisionId, offset, dim);
}

//-------------------------------------------------------------------
void Meta::getData(std::vector<float>& data, const exec::Params* params) const {
	// get context
	Context& ctx = Context::get();

	// resize vector
	data.resize(m_meta.size());

	// init vars
	uint32_t i = 0;
	
	// set meta data
	for(const Item& meta : m_meta) {
		if(params && meta.decisionId() == params->decisionId) {
			if(meta.dim() == 0)			data[i] = (float)params->grid.x;
			else if(meta.dim() == 1)	data[i] = (float)params->grid.y;
			else if(meta.dim() == 2)	data[i] = (float)params->grid.z;
			else if(meta.dim() == 3)	data[i] = (float)params->block.x;
			else if(meta.dim() == 4)	data[i] = (float)params->block.y;
			else if(meta.dim() == 5)	data[i] = (float)params->block.z;
		}
		else {
			data[i] = (float)ctx.getMetaValue(meta.decisionId(), meta.offset(), meta.dim());
		}

		i++;
	}
}

//-------------------------------------------------------------------
			}
		}
	}
}