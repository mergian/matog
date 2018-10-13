// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Predictor.h"
#include "predictor/Meta.h"
#include "Context.h"
#include "BaseArray.h"

#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/Kernel.h>
#include <matog/GPU.h>
#include <matog/Static.h>

#include <matog/util/ml/Model.h>
#include <matog/util/ml/Type.h>

#include <matog/macros.h>
#include <matog/log.h>

using namespace matog::util::ml;
using namespace matog::db;
using namespace matog::kernel;

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
Predictor::Models		Predictor::s_models;

//-------------------------------------------------------------------
bool Predictor::init(void) {
	Stmt stmt("SELECT decision_id, type, model FROM " DB_MODELS);
	Stmt meta_stmt("SELECT meta_decision_id, meta_decision_offset, dim FROM " DB_MODEL_FIELDS " WHERE decision_id = ? ORDER BY ordering ASC;");
	
	while(stmt.step()) {
		predictor::Meta* meta = 0;

		// load model
		Model* model = Model::create(stmt.get<util::ml::Type>(1));
		model->load(stmt.get<util::blob>(2));

		// save model
		const uint32_t decisionId = stmt.get<uint32_t>(0);
		meta = &s_models.emplace(MAP_EMPLACE(decisionId, model)).first->second;
			
		// load meta
		if(model->getType() != util::ml::Type::NONE) {
			meta_stmt.bind(0, stmt.get<uint32_t>(0));
			
			while(meta_stmt.step()) {
				const uint32_t metaDecisionId		= meta_stmt.get<uint32_t>(0);
				const uint32_t metaDecisionOffset	= meta_stmt.get<uint32_t>(1);
				const uint32_t dim					= meta_stmt.get<uint32_t>(2);
				
				meta->addMeta(metaDecisionId, metaDecisionOffset, dim);
			}

			meta_stmt.reset();
		}
	}
	
	return !s_models.empty();
}

//-------------------------------------------------------------------
uint32_t Predictor::predict(const uint32_t decisionId, const exec::Params* params) {
	auto it = s_models.find(decisionId);

	// return baseline if no model is available. This can happen when
	// the array is only used for an D->H copy, but the user initializes
	// the data.
	if(it == s_models.end()) {
		L_WARN("no model for decision %u found!", decisionId);
		return 0;
	}
	
	const Model* model = it->second.getModel();

	if(model->getType() != util::ml::Type::NONE) {
		std::vector<float> data;
		it->second.getData(data, params);

		return model->predict(&data[0]);
	}

	return model->predict(0);
}

//-------------------------------------------------------------------
		}
	}
}