// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "PredictionModel.h"
#include <matog/db/tables.h>
#include <matog/db/Stmt.h>
#include <matog/util/ml/Model.h>
#include <cassert>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
void PredictionModel::dbDeleteAll(void) {
	Stmt("DELETE FROM " DB_MODELS ";").step();
	Stmt("DELETE FROM " DB_MODEL_FIELDS ";").step();
	Stmt("DELETE FROM sqlite_sequence WHERE NAME = '" DB_MODELS "';").step();
}

//-------------------------------------------------------------------
void PredictionModel::dbInsertModel(const uint32_t decisionId, const util::ml::Model* model, const uint32_t* decisionIds, const uint32_t* offsets, const uint32_t* dims, const uint32_t metaCnt) {
	// insert model
	Stmt stmt("INSERT INTO " DB_MODELS " (decision_id, type, model) VALUES (?, ?, ?);");
	stmt.bind(0, decisionId);
	stmt.bind(1, (int)model->getType());
	
	const util::blob blob = model->save();
	stmt.bind(2, blob);
	stmt.step();
	FREE_A(blob.ptr);

	// insert meta ids
	if(metaCnt) {
		assert(decisionIds);
		assert(offsets);
		assert(dims);

		stmt("INSERT INTO " DB_MODEL_FIELDS " (decision_id, ordering, meta_decision_id, meta_decision_offset, dim) VALUES (?, ?, ?, ?, ?);");
		stmt.bind(0, decisionId);

		for(uint32_t i = 0; i < metaCnt; i++) {
			stmt.reset();
			stmt.bind(1, i);
			stmt.bind(2, decisionIds[i]);
			stmt.bind(3, offsets[i]);
			stmt.bind(4, dims[i]);
			stmt.step();
		}
	}
}

//-------------------------------------------------------------------
	}
}