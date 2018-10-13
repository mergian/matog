// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Prediction.h"
#include "Helper.h"
#include <matog/log.h>
#include <matog/analyze/graph/Graph.h>
#include <matog/analyze/graph/call/Item.h>
#include <matog/analyze/algorithm/Algorithm.h>
#include <matog/event/ProfilingAlgorithm.h>
#include <matog/Options.h>
#include <matog/Variant.h>
#include <matog/Array.h>
#include <matog/Degree.h>
#include <matog/degree/Type.h>
#include <matog/Static.h>
#include <matog/macros.h>
#include <tbb/tbb.h>
#include <cfloat>

using namespace matog::analyze::graph;

namespace matog {
	namespace analyze {
		namespace algorithm {
			namespace prediction {		
//-------------------------------------------------------------------
Prediction::Prediction(const graph::Graph& graph) : m_graph(graph) {
	L_DEBUG_
	printf("using ");
	log::setColor(log::TEAL);
	printf("PREDICTION");
	log::resetColor();
	printf(" algorithm");
	L_END

	if(m_graph.getAlgorithm() != event::ProfilingAlgorithm::Prediction)
		L_WARN("FYI: Using brute force profiling data with the prediction algorithm is not very helpful, as most of the data is ignored!");

	initLocal();
	processGlobal();
	processLocal();
}

//-------------------------------------------------------------------
void Prediction::initLocal(void) {
	// for all calls
	for(Call* call : m_graph.getCalls()) {
		// get 
		auto& local = m_local[call];

		variant::List domains;
		Helper::getDomains(domains, call);

		// for all domains
		for(const variant::Hash domain : domains)
			local.emplace(MAP_EMPLACE(domain, Helper::findBestHash(domain, call)));
	}
}

//-------------------------------------------------------------------
void Prediction::processGlobal(void) {
	// for all decisions
	for(Decision* decision : m_graph.getDecisions()) {
		// init
		const Variant& aVariant = decision->getArray()->getVariant();
		variant::Hash arrayHash = 0;

		// for all degrees
		for(const Item& item : aVariant.subset(variant::Type::Global)) {
			const Degree* degree = item.getDegree();

			// init
			double			bestTotalTime	= DBL_MAX;
			variant::Value	bestValue		= 0;

			// for all values
			for(variant::Value value = 0; value < degree->getValueCount(); value++) {
				// init
				double totalTime = 0.0;

				// for all calls
				for(Call* call : m_graph.getCalls()) {
					// skip if this decision is not used in the call
					if(call->getParents().find(decision) == call->getParents().end())
						continue;

					// init
					const auto& variants	= call->getKernel()->getVariant();
					const call::Item		base		= call->getItem(0);
					double					localTime	= base.time;

					// we have to iterate over the paramIds as it is possible, that there are multiple 
					// instances of the same decision.
					const auto range = call->getParamIds().equal_range(decision);
					for(auto rit = range.first; rit != range.second; rit++) {
						const uint32_t		paramId	= rit->second;
						const variant::Hash support	= variants.hash(degree, paramId, value);
						const call::Item	item	= call->getItem(support);
						assert(item.isValid()); // has to exist!

						localTime += item.time - base.time;
					}

					// add to overallTime
					totalTime += localTime;
				}

				// is best?
				if(totalTime < bestTotalTime) {
					bestTotalTime	= totalTime;
					bestValue		= value;
				}
			}

			// save
			arrayHash += aVariant.hash(item, bestValue);
		}

		// save
		decision->setFinalConfig(arrayHash);
	}
}

//-------------------------------------------------------------------
void Prediction::processLocal(void) {
	double total = 0.0;

	// for each call
	for(Call* call : m_graph.getCalls()) {
		const auto& variants	= call->getKernel()->getVariant();
		double bestTime			= DBL_MAX;
		variant::Hash bestLocal	= 0;
		variant::Hash global	= 0;

		// for all domains
		for(auto& it : m_local[call]) {
			double time = it.second.second; // init local time
			
			const variant::Hash domain	= it.first;
			const call::Item base		= call->getItem(domain);
			assert(base.isValid()); // has to exist

			// for all decisions
			for(const auto& it : call->getParamIds()) {
				Decision* decision		= it.first;
				const Variant& aVariant	= decision->getArray()->getVariant();
				const uint32_t paramId	= it.second;

				// for all global degrees
				for(const Item& item : aVariant.subset(variant::Type::Global)) {
					const variant::Value value = aVariant.get(decision->getFinalConfig(), item);
					const variant::Hash  hash  = variants.hash(item.getDegree(), paramId, value);

					// only in first run
					if(bestTime == DBL_MAX)
						global += hash;

					const call::Item support = call->getItem(domain + hash);
					assert(support.isValid()); // has to exist

					time += support.time - base.time;
				}
			}

			if(time < bestTime) {
				bestTime  = time;
				bestLocal = it.second.first;
			}
		}

		// save
		call->setFinalConfig(global + bestLocal);
		total += bestTime;
	}

	C_DEBUG(log::TEAL, "Estimated time: %fms", total);
}

//-------------------------------------------------------------------
			}
		}
	}
}