// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Profiler.h"
#include "profiler/Plain.h"
#include "profiler/Brute.h"
#include "profiler/Prediction.h"
#include "profiler/PredictionRandom.h"
#include <matog/runtime/intern/Runtime.h>

#include <matog/Options.h>
#include <matog/event/ProfilingAlgorithm.h>

#include <matog/util/Exit.h>
#include <matog/util/Mem.h>

#include <matog/log.h>
#include <matog/macros.h>

using namespace matog::kernel;
using namespace matog::util;

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
event::ProfilingAlgorithm								Profiler::s_algorithm = event::ProfilingAlgorithm::Prediction;
uint32_t												Profiler::s_profilingId = 0;
std::map<const Kernel*, uint32_t>						Profiler::s_kernelRunCounts;
uint32_t												Profiler::s_metricCount = 0;
std::map<CUdevice, std::vector<CUpti_MetricID> >		Profiler::s_metricIds;
profiler::Converter										Profiler::s_converter;
Profiler::Instances										Profiler::s_instances;

//-------------------------------------------------------------------
void Profiler::addCuptiInstance(const uint32_t id, profiler::CUPTI* data) {
	LOCK(s_instances.mutex);
	s_instances.map.emplace(id, data);
}

//-------------------------------------------------------------------
void Profiler::setCuptiValues(const uint32_t id, const uint64_t duration, const L1Cache requested, const L1Cache executed) {
	LOCK(s_instances.mutex);
	auto it = s_instances.map.find(id);

	THROWIF(it == s_instances.map.end(), "UNKNOWN_ERROR");

	it->second->setDuration(duration);
	it->second->setRequestedCache(requested);
	it->second->setExecutedCache(executed);
	s_instances.map.erase(it);	
}

//-------------------------------------------------------------------
event::ProfilingAlgorithm Profiler::getAlgorithm(void) {
	return s_algorithm;
}

//-------------------------------------------------------------------
void Profiler::init(void) {
	// init CUPTI
	initCupti();
	initMetrics();

	// init default algorithm
	s_algorithm = Options::get<event::ProfilingAlgorithm>({"profiling", "algorithm"}, event::ProfilingAlgorithm::Prediction, [](const char* algorithm) {
		if(strcmp(algorithm, "plain") == 0)				
			return event::ProfilingAlgorithm::Plain;
		else if(strcmp(algorithm, "brute") == 0)
			return event::ProfilingAlgorithm::Brute;
		else if(strcmp(algorithm, "prediction") == 0)
			return event::ProfilingAlgorithm::Prediction;
		else if(strcmp(algorithm, "predictionrandom") == 0)
			return event::ProfilingAlgorithm::PredictionRandom;
	
		THROW("Unknown profiler. Available profilers are: plain, brute and prediction");
		return event::ProfilingAlgorithm::UNKNOWN;	
	});

	// init profiling
	s_profilingId = event::dbInsertProfiling(getAlgorithm());
		
	// does profiling exist?
	THROWIF(getId() == 0, "NO_ACTIVE_PROFILING");

	// store config
	Options::dbInsert(getId());

	// add exit hook
	Exit::add(&Profiler::finalize);
}

//-------------------------------------------------------------------
void Profiler::initCupti(void) {
	// enable activity: kernels
	CHECK(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_KERNEL));

	// register callbacks
	CHECK(cuptiActivityRegisterCallbacks(&profiler::CUPTI::cuptiActivityBufferRequestedCallback, &profiler::CUPTI::cuptiActivityBufferCompletedCallback));
}

//-------------------------------------------------------------------
uint32_t Profiler::getMetricCount(void) {
	return s_metricCount;
}

//-------------------------------------------------------------------
CUpti_MetricID* Profiler::getMetricIds(CUdevice device) {
	const auto it = s_metricIds.find(device);
	THROWIF(it == s_metricIds.end(), "UNKNOWN_DEVICE");
	return &it->second.front();
}

//-------------------------------------------------------------------
void Profiler::initMetrics(void) {
	Json::Value metricConfig = Options::get<Json::Value>({"profiling", "metrics"}, Json::Value::nullSingleton());

	if(metricConfig.isArray()) {
		s_metricCount = (uint32_t)metricConfig.size();
		
		int deviceCount = 0;
		CHECK(cuDeviceGetCount(&deviceCount));

		for(CUdevice device = 0; device < deviceCount; device++) {
			std::vector<CUpti_MetricID>& ids = s_metricIds.emplace(MAP_EMPLACE(device, s_metricCount)).first->second;

			uint32_t i = 0;
			for(Json::Value value : metricConfig) {
				assert(i < s_metricCount);
				THROWIF(!value.isString(), "UNKNOWN_VALUE");

				const char* metricString = value.asCString();
				CHECK(cuptiMetricGetIdFromName(device, metricString, &ids[i++]));
			}
		}
	}
}

//-------------------------------------------------------------------
void Profiler::finalize(void) {
	event::printSummary(getId());
	event::dbUpdateProfilingDuration(getId());
}

//-------------------------------------------------------------------
Profiler* Profiler::getProfiler(Params& params) {
	switch(getAlgorithm()) {
	case event::ProfilingAlgorithm::Plain:
		return NEW(profiler::Plain(params));
	case event::ProfilingAlgorithm::Brute:	
		return NEW(profiler::Brute(params));
	case event::ProfilingAlgorithm::Prediction:
		return NEW(profiler::Prediction(params));
	case event::ProfilingAlgorithm::PredictionRandom:
		return NEW(profiler::PredictionRandom(params));
	default: 
		break;
	};
	
	THROW("UNKNOWN_PROFILER");
	return 0;
}

//-------------------------------------------------------------------
			}
		}
	}
}