// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_DB_PREDICTIONMODEL
#define __MATOG_DB_PREDICTIONMODEL

#include <matog/dll.h>
#include <matog/util.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL PredictionModel {
public:
	static void dbInsertModel	(const uint32_t decisionId, const util::ml::Model* model, const uint32_t* decisionIds, const uint32_t* offsets, const uint32_t* dims, const uint32_t metaCnt);
	static void dbDeleteAll		(void);
};

//-------------------------------------------------------------------
	}
}

#endif