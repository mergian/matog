// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/analyze/training/Training.h>
#include <matog/analyze/training/Data.h>
#include <matog/analyze/graph/Graph.h>
#include <matog/log.h>
#include <matog/Array.h>
#include <matog/Options.h>
#include <matog/db/PredictionModel.h>
#include <matog/Kernel.h>
#include <matog/GPU.h>
#include <matog/util/ml/Type.h>
#include <matog/util/ml/Model.h>
#include <matog/macros.h>

using namespace matog::util;
using namespace matog::analyze::graph;

namespace matog {
	namespace analyze {
		namespace training {
//-------------------------------------------------------------------
bool s_isLog = false;

//-------------------------------------------------------------------
bool Training::isLog(void) {
	return s_isLog;
}

//-------------------------------------------------------------------
void Training::exec(void) {
	s_isLog = Options::get<bool>({"log", "training"}, s_isLog);
	arrays();
	calls();
}

//-------------------------------------------------------------------
void Training::arrays(void) {
	C_INFO(log::GREEN, "-- training arrays");

	// get default models
	const ml::Type modelType = Options::get<ml::Type>({"analyze", "training", "array_model"}, ml::Type::SVM_LINEAR, &ml::getTypeByName);

	// iterate all calls and map them to kernels
	std::map<uint32_t, Data::List> arrays;
	std::map<uint32_t, const Array*> arrayLookup;
	
	for(auto& it : Graph::getAllDecisions()) {
		Decision* decision = &it.second;
		arrays[decision->getDecisionId()].emplace_back(std::make_pair(decision->getId(), decision->getFinalConfig()));
		arrayLookup.emplace(MAP_EMPLACE(decision->getDecisionId(), decision->getArray()));
	}

	// iterate all kernels and build models
	for(const auto& it : arrays) {
		L_DEBUG("training decision %u...", it.first);

		// reduce data
		Data data(it.second, arrayLookup.at(it.first)->getVariant(), true);

		// create models
		ml::Type type = modelType;

		if(data.getRows() == 1)
			type = ml::Type::NONE;

		ml::Model* model = ml::Model::create(type);

		// train
		model->train(data.getInput(), data.getOutput(), data.getCols(), data.getRows(), data.getCounts());

		// save to db
		db::PredictionModel::dbInsertModel(it.first, model, data.getDecisionIds(), data.getOffsets(), data.getDims(), data.getCols());

		// free model
		FREE(model);
	}
}

//-------------------------------------------------------------------
void Training::calls(void) {
	C_INFO(log::GREEN, "-- training calls");

	// get default models
	const ml::Type defaultType = Options::get<ml::Type>({"analyze", "training", "kernel_model"}, ml::Type::SVM_LINEAR, &ml::getTypeByName);

	// iterate all calls and map them to kernels
	std::map<uint32_t, Data::List> kernels;
	std::map<uint32_t, ml::Type> types;
	std::map<uint32_t, const Kernel*> kernelLookup;
	
	Json::Value json = Options::get<Json::Value>({"analyze", "training", "kernels"}, Json::Value::nullSingleton());

	for(auto& it : Graph::getAllCalls()) {
		Call* call						= &it.second;
		const uint32_t decisionId		= call->getDecisionId();
		const uint32_t eventId			= call->getId();
		const Kernel* kernel			= call->getKernel();

		// add to map
		kernels[decisionId].emplace_back(std::make_pair(eventId, call->getFinalConfig()));
		
		kernelLookup.emplace(MAP_EMPLACE(decisionId, kernel));

		// figure out ML::Type
		ml::Type type = defaultType;

		if(json.isObject()) {
			Json::Value item = json.get(kernel->getName(), Json::Value());
			
			if(item.isObject()) {
				Json::Value algorithm = item.get("model", Json::Value());

				if(algorithm.isString())
					type = ml::getTypeByName(algorithm.asString().c_str());
			}
		}

		types.emplace(MAP_EMPLACE(decisionId, type));
	}

	// iterate all kernels and build models
	for(const auto& it : kernels) {
		L_DEBUG("Kernel decision %u", it.first);

		// reduce data
		Data data(it.second, kernelLookup.at(it.first)->getVariant(), false);

		// create models
		ml::Type type = types[it.first];

		if(data.getRows() == 1)
			type = ml::Type::NONE;

		ml::Model* model = ml::Model::create(type);

		// train
		model->train(data.getInput(), data.getOutput(), data.getCols(), data.getRows(), data.getCounts());

		// save to db
		db::PredictionModel::dbInsertModel(it.first, model, data.getDecisionIds(), data.getOffsets(), data.getDims(), data.getCols());

		// free model
		FREE(model);
	}
}

//-------------------------------------------------------------------
		}
	}
}