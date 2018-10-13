// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Brute.h"
#include <matog/analyze/graph/Graph.h>
#include <matog/analyze/graph/Decision.h>
#include <matog/analyze/algorithm/Algorithm.h>
#include <matog/analyze/graph/call/Item.h>
#include <matog/analyze/algorithm/prediction/Helper.h>
#include <matog/event/ProfilingAlgorithm.h>
#include <matog/Variant.h>
#include <matog/Array.h>
#include <matog/Degree.h>
#include <matog/Kernel.h>
#include <matog/log.h>
#include <matog/macros.h>
#include <tbb/tbb.h>
#include <cfloat>

using namespace matog::analyze::graph;
using matog::analyze::algorithm::prediction::Helper;

#define USE_TBB

namespace matog {
	namespace analyze {
		namespace algorithm {
			namespace brute {		
//-------------------------------------------------------------------
Brute::Brute(const Graph& graph) : m_graph(graph) {
	L_DEBUG_
	printf("using ");
	log::setColor(log::TEAL);
	printf("BRUTE/%s", m_graph.getAlgorithm() == event::ProfilingAlgorithm::Prediction ? "PREDICTION" : "BRUTE");
	log::resetColor();
	printf(" algorithm");
	L_END

	initCalls();
	initVariants();
	process();
}

//-------------------------------------------------------------------
void Brute::initCalls(void) {
	L_DEBUG("precomputing calls...");
	m_calls.reserve(m_graph.getCalls().size());

	// init all
	for(graph::Call* call : m_graph.getCalls())
		m_calls.emplace(MAP_EMPLACE(call));		

	// perform precalculations
	#ifdef USE_TBB
		tbb::task_group group;
	
		for(graph::Call* call : m_graph.getCalls()) {
			auto it = m_calls.find(call);
			assert(it != m_calls.end());

			if(m_graph.getAlgorithm() == event::ProfilingAlgorithm::Prediction)
				group.run([it, this]{initCallPrediction(it);});
			else
				group.run([it, this]{initCallBrute(it);});			
		}

		group.wait();
	#else
	for(graph::Call* call : m_graph.getCalls()) {
		auto it = m_calls.find(call);
		assert(it != m_calls.end());

		if(m_graph.getAlgorithm() == event::ProfilingAlgorithm::Prediction)
			initCallPrediction(it);
		else
			initCallBrute(it);
	}
	#endif
}

//-------------------------------------------------------------------
void Brute::initCallPrediction(TimeMap::iterator callIt) {
	Call* call				= callIt->first;
	const Kernel* kernel	= call->getKernel();
	const Variant& variants	= kernel->getVariant();
	auto& global			= callIt->second;

	// init global hashes
	global.resize(variants.count(variant::Type::Global), std::make_pair(0, DBL_MAX));

	// calc domains and remove if have not been executed
	variant::List domains;
	Helper::getDomains(domains, call);
	
	// go through all combinations
	for(variant::CompressedHash globalCompressed = 0; globalCompressed < global.size(); globalCompressed++) {
		const variant::Hash globalHash = variants.uncompress(globalCompressed, variant::Type::Global);

		// for all domains
		for(const variant::Hash domain : domains) {
			const variant::Hash localHash	= Helper::findBestHash(domain, call).first;
			const variant::Hash hash		= globalHash + localHash;

			const double predicted	= Helper::predict(hash, call);
			auto& item = global[globalCompressed];

			if(predicted < item.second)
				item = std::make_pair(hash, predicted);
		}
	}	
}

//-------------------------------------------------------------------
void Brute::initCallBrute(TimeMap::iterator it) {
	Call* call				= it->first;
	const Kernel* kernel	= call->getKernel();
	const Variant& variants	= kernel->getVariant();
	auto& global			= it->second;

	// init global hashes
	global.resize(variants.count(variant::Type::Global), std::make_pair(0, DBL_MAX));

	// go through all profiled results
	const variant::Hash cnt = variants.count();
	for(variant::Hash hash = 0; hash < cnt; hash++) {
		const call::Item item = call->getItem(hash);

		if(!item.isValid())
			continue;
		
		const variant::CompressedHash globalHash = variants.compress(hash, variant::Type::Global);

		auto& pair = global[globalHash];
	
		if(item.time < pair.second)
			pair = std::make_pair(hash, item.time);
	}
}

//-------------------------------------------------------------------
void Brute::initVariants(void) {
	L_DEBUG("init variants...");

	uint32_t cnt = 0;

	// generate ids and calc item count
	for(Decision* d : m_graph.getDecisions()) {
		m_decisionIds.emplace(MAP_EMPLACE(d, (uint32_t)m_decisionIds.size()));
		cnt += (uint32_t)d->getArray()->getVariant().subset(variant::Type::Global).size();
	}

	// add to items
	for(const auto& it : m_decisionIds) {
		const auto& globalDegrees = it.first->getArray()->getVariant().subset(variant::Type::Global);

		for(const Item& item : globalDegrees)
			m_items.emplace(item.getDegree(), it.second);
	}

	// init variants
	m_variants.init(m_items);
}

//-------------------------------------------------------------------
variant::CompressedHash Brute::getGlobalHash(const variant::Hash hash, const graph::Call* call) const {
	variant::Hash globalHash = 0;

	const Variant& callVariants = call->getKernel()->getVariant();

	for(auto it = call->getParamIds().begin(); it != call->getParamIds().end(); it++) {
		Decision*		decision	= it->first;
		const uint32_t	paramId		= it->second;
		const uint32_t	decisionId	= m_decisionIds.find(decision)->second; // has to exist

		for(const Item& item : decision->getArray()->getVariant().subset(variant::Type::Global)) {
			const variant::Value value = m_variants.get(hash, item.getDegree(), decisionId);
			globalHash += callVariants.hash(item.getDegree(), paramId, value);
		}
	}

	return callVariants.compress(globalHash, variant::Type::Global);
}

//-------------------------------------------------------------------
void Brute::process(void) {
	L_DEBUG("processing global decisions...");
	const variant::Hash cnt = m_variants.count();

	struct Best {
		variant::Hash hash;
		double time;

		inline Best(void) : hash(0), time(DBL_MAX) {}
		inline Best(const variant::Hash _hash) : hash(_hash), time(0.0) {}

		inline bool operator<(const Best& o) const {
			return time < o.time;
		}

		inline void operator=(const Best& o) {
			hash = o.hash;
			time = o.time;
		}
	};

	Best best;
	
	#ifdef USE_TBB
		tbb::combinable<Best> tbbBest;

		tbb::parallel_for(variant::Hash(0), cnt, [&](const variant::Hash hash) {
			auto& local = tbbBest.local();
			Best current(hash);

			for(const auto& it : m_calls) {
				const variant::CompressedHash globalHash = getGlobalHash(hash, it.first);
				current.time += it.second[globalHash].second;
			}
		
			L_INSANE_
			printf("Hash: %llu / Time: %12.3f /", (unsigned long long int)current.hash, current.time);
			m_variants.print(current.hash);
			L_END

			if(current < local)
				local = current;
		});

		best = tbbBest.combine([&](const Best& A, const Best& B) {
			if(A < B)
				return A;
			return B;
		});
	#else
		for(variant::Hash hash = 0; hash < cnt; hash++) {
			if(log::isTrace() && hash % 10000 == 0)
				L_TRACE("%6.2f%% // %llu / %llu", hash / (double)cnt * 100.0, (unsigned long long int)hash, (unsigned long long int)cnt);

			Best current(hash);

			for(const auto& it : m_calls) {
				const variant::CompressedHash globalHash = getGlobalHash(hash, it.first);
				current.time += it.second[globalHash].second;
			}
		
			L_INSANE_
			printf("Hash: %llu / Time: %12.3f /", (unsigned long long int)current.hash, current.time);
			m_variants.print(current.hash);
			L_END

			if(current < best)
				best = current;
		}
	#endif

	assert(best.time != DBL_MAX);

	setBest(best.hash);
	C_INFO(log::TEAL, "Estimated time: %fms", best.time);
}

//-------------------------------------------------------------------
void Brute::setBest(const variant::Hash hash) {
	// for calls
	for(const auto& it : m_calls) {
		const variant::CompressedHash globalHash = getGlobalHash(hash, it.first);
		it.first->setFinalConfig(it.second[globalHash].first);
	}
	
	// for decisions
	for(Decision* decision : m_graph.getDecisions()) {
		const Variant& aVariant		= decision->getArray()->getVariant();
		const uint32_t decisionId	= m_decisionIds.find(decision)->second; // has to exist
		variant::Hash hash = 0;
		
		for(const Item& item : aVariant.subset(variant::Type::Global))
			hash += aVariant.hash(item.getDegree(), 0, m_variants.get(hash, item.getDegree(), decisionId));
		
		decision->setFinalConfig(hash);
	}
}

//-------------------------------------------------------------------
			}
		}
	}
}