// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Helper.h"
#include <matog/log.h>
#include <matog/analyze/graph/call/Item.h>
#include <matog/analyze/graph/Call.h>
#include <matog/event/ProfilingAlgorithm.h>
#include <matog/Options.h>
#include <matog/Variant.h>
#include <matog/Array.h>
#include <matog/Degree.h>
#include <matog/degree/Type.h>
#include <matog/Static.h>
#include <matog/macros.h>
#include <cfloat>

using namespace matog::analyze::graph;

namespace matog {
	namespace analyze {
		namespace algorithm {
			namespace prediction {		
//-------------------------------------------------------------------
Helper::Domains Helper::s_domains;

//-------------------------------------------------------------------
void Helper::getDomains(variant::List& domains, const graph::Call* call) {
	assert(call);

	auto it = s_domains.find(call->getKernel());
	assert(it != s_domains.end());

	for(const variant::Hash domain : it->second) {
		if(call->getItem(domain).isValid())
			domains.emplace_back(domain);
	}
}

//-------------------------------------------------------------------
void Helper::initDomains(void) {
	// is already initialized?
	if(!s_domains.empty())
		return;

	L_INFO("precomputing domains...");

	// for all kernels
	for(const auto& kit : Static::getKernels()) {
		const auto& variants		= kit.second.getVariant();
		auto& domains				= s_domains[&kit.second];

		variants.predictionDomains(domains);
	}
}

//-------------------------------------------------------------------
std::pair<variant::Hash, double> Helper::findBestHash(const variant::Hash domain, const graph::Call* call) {
	const Kernel* kernel					= call->getKernel();
	const Variant& variants					= kernel->getVariant();
	const variant::Set& localIndependent	= variants.subset(variant::Type::LocalIndependent);

	const call::Item base = call->getItem(domain);
	assert(base.isValid());

	std::pair<variant::Hash, double> result = std::make_pair(domain, base.time);

	for(const variant::Item& item : localIndependent) {
		const auto bestValue = findBestValue(domain, call, item);

		result.first	+= variants.hash(item, bestValue.first);
		result.second	+= bestValue.second - base.time;
	}

	return result;
}

//-------------------------------------------------------------------
std::pair<variant::Value, double> Helper::findBestValue(const variant::Hash domain, const graph::Call* call, const variant::Item& item) {
	assert(item.getDegree()->isLocal());
	assert(item.getDegree()->isIndependent());

	const Variant&		variants	= call->getKernel()->getVariant();
	const call::Item	base		= call->getItem(domain);

	call::Item			bestItem	= base;
	variant::Value		bestValue	= 0;

	// for all values
	for(variant::Value value = 1; value < item.getDegree()->getValueCount(); value++) {
		const call::Item support = call->getItem(domain + variants.hash(item, value));
					
		// can e.g. happen for memory degrees
		if(!support.isValid()) 
			continue;

		if(support < bestItem) {
			bestItem	= support;
			bestValue	= value;
		}
	}

	return std::make_pair(bestValue, bestItem.time);
}

//-------------------------------------------------------------------
double Helper::predict(const variant::Hash hash, const graph::Call* call) {
	const Variant& variants = call->getKernel()->getVariant();

	const variant::Hash domain = variants.convert(hash, variant::Type::Shared);
	const call::Item base = call->getItem(domain);
	const call::Item zeroBase = call->getItem(0);

	double time = base.time;

	for(const auto& item : variants.subset(variant::Type::Independent)) {
		const variant::Value value = variants.get(hash, item);

		// only necessary if value != 0, as 0 is baseline.
		if(value) {
			const double baseTime = item.getDegree()->isGlobal() ? zeroBase.time : base.time;
			const variant::Hash baseDomain = item.getDegree()->isGlobal() ? 0 : domain;
			
			const call::Item support = call->getItem(baseDomain + variants.hash(item, value));
			assert(support.isValid());
			time += support.time - baseTime;
		}
	}

	return time;
}

//-------------------------------------------------------------------
			}
		}
	}
}