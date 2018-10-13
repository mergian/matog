// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CUPTI.h"
#include <matog/runtime/intern/exec/Profiler.h>
#include <matog/runtime/intern/Runtime.h>
#include <matog/util/Mem.h>
#include <matog/log.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
void CUPTIAPI CUPTI::cuptiActivityCallback(void* userdata, CUpti_CallbackDomain domain, CUpti_CallbackId cbid, const CUpti_CallbackData* cbdata) {
	if(domain == CUPTI_CB_DOMAIN_DRIVER_API && 
		cbdata->callbackSite == CUPTI_API_ENTER &&
		cbid == CUPTI_DRIVER_TRACE_CBID_cuLaunchKernel) {
		Profiler::addCuptiInstance(cbdata->correlationId, (CUPTI*)userdata);
	}
}

//-------------------------------------------------------------------
void CUPTIAPI CUPTI::cuptiActivityBufferRequestedCallback(uint8_t** buffer, size_t* size, size_t* maxNumRecords) {
	*size = 16 * 1024 * sizeof(uint64_t);
	*buffer = (uint8_t*)NEW_A(uint64_t, *size / sizeof(uint64_t));
	*maxNumRecords = 0;

	THROWIF(*buffer == 0, "OUT_OF_MEMORY");
}

//-------------------------------------------------------------------
void CUPTIAPI CUPTI::cuptiActivityBufferCompletedCallback(CUcontext ctx, uint32_t streamId, uint8_t* buffer, size_t size, size_t validSize) {
	CUpti_Activity *record = NULL;

	assert(validSize);

	CHECK(cuptiActivityGetNextRecord(buffer, validSize, &record));
		
	if(record->kind == CUPTI_ACTIVITY_KIND_KERNEL) {
		uint64_t duration = 0;
		L1Cache requested = L1Cache::SM, executed = L1Cache::SM;
		uint32_t id = 0;
		uint32_t version;
		CHECK(cuptiGetVersion(&version));

		switch(version) {
			// in CUPTI5+6 we use AKv2
			case 5:
			case 6: {
				CUpti_ActivityKernel2* kernel = (CUpti_ActivityKernel2*)record;
				duration = kernel->end - kernel->start;
				id = kernel->correlationId;
				executed = (L1Cache)(kernel->cacheConfig.config.executed - 1);
				requested = (L1Cache)(kernel->cacheConfig.config.requested - 1);
			} break;
		
			// since CUPTI7 we use AKv3
			#if CUPTI_API_VERSION >= 7
			case 7:
			case 8:
			case 9:
			{
				CUpti_ActivityKernel3* kernel = (CUpti_ActivityKernel3*)record;
				duration = kernel->end - kernel->start;
				id = kernel->correlationId;
				executed = (L1Cache)(kernel->cacheConfig.config.executed - 1);
				requested = (L1Cache)(kernel->cacheConfig.config.requested - 1);
			} break;
			#endif
			
			// error
			default: {
				THROW("CUPTI Version not supported!");
			}
		}

		Profiler::setCuptiValues(id, duration, requested, executed);
	}
	
	if(buffer)
		FREE_A(buffer);
}

//-------------------------------------------------------------------
void CUPTIAPI CUPTI::cuptiMetricCallback(void *userdata, CUpti_CallbackDomain domain, CUpti_CallbackId cbid, const CUpti_CallbackData *cbInfo) {
	// get metric data package
	CUPTI& data = *(CUPTI*)userdata;

	if(cbid != CUPTI_DRIVER_TRACE_CBID_cuLaunchKernel) {
		L_ERROR("Unknown CUPTI Metric CBID");
		return;
	}

	// get EventGroupSet
	assert(data.cuptiSets());
	assert(data.cuptiCounter() < data.cuptiSets()->numSets);
	CUpti_EventGroupSet* eventGroupSet = &data.cuptiSets()->sets[data.cuptiCounter()];

	// on entry
	if(cbInfo->callbackSite == CUPTI_API_ENTER) {
		// sync
		CHECK(cuCtxSynchronize());

		// set collection mode
		CHECK(cuptiSetEventCollectionMode(cbInfo->context, CUPTI_EVENT_COLLECTION_MODE_KERNEL));

		// enable event groups
		for(uint32_t i = 0; i < eventGroupSet->numEventGroups; i++) {
			uint32_t all = 1;
			CHECK(cuptiEventGroupSetAttribute(eventGroupSet->eventGroups[i], CUPTI_EVENT_GROUP_ATTR_PROFILE_ALL_DOMAIN_INSTANCES, sizeof(all), &all));
			CHECK(cuptiEventGroupEnable(eventGroupSet->eventGroups[i]));
		}
	}

	// on exit
	if(cbInfo->callbackSite == CUPTI_API_EXIT) {
		CHECK(cuCtxSynchronize());

		// extract information
		for(uint32_t i = 0; i < eventGroupSet->numEventGroups; i++) {
			// get 'some' cupti header data
			CUpti_EventGroup group = eventGroupSet->eventGroups[i];
			CUpti_EventDomainID groupDomain;
			uint32_t numEvents, numInstances, numTotalInstances;
			
			size_t groupDomainSize			= sizeof(groupDomain);
			size_t numEventsSize			= sizeof(numEvents);
			size_t numInstancesSize			= sizeof(numInstances);
			size_t numTotalInstancesSize	= sizeof(numTotalInstances);

			CHECK(cuptiEventGroupGetAttribute(group, CUPTI_EVENT_GROUP_ATTR_EVENT_DOMAIN_ID, &groupDomainSize, &groupDomain));
			CHECK(cuptiDeviceGetEventDomainAttribute(data.getDeviceId(), groupDomain, CUPTI_EVENT_DOMAIN_ATTR_INSTANCE_COUNT, &numTotalInstancesSize, &numTotalInstances));
			CHECK(cuptiEventGroupGetAttribute(group, CUPTI_EVENT_GROUP_ATTR_INSTANCE_COUNT, &numInstancesSize, &numInstances));
			CHECK(cuptiEventGroupGetAttribute(group, CUPTI_EVENT_GROUP_ATTR_NUM_EVENTS, &numEventsSize, &numEvents));
			
			// get event ids
			CUpti_EventID* eventIds = NEW_A(CUpti_EventID, numEvents);
			size_t eventIdsSize = numEvents * sizeof(CUpti_EventID);
			CHECK(cuptiEventGroupGetAttribute(group, CUPTI_EVENT_GROUP_ATTR_EVENTS, &eventIdsSize, eventIds));

			// prepare values
			uint64_t* values = NEW_A(uint64_t, numInstances);
			uint64_t normalized;
			uint64_t sum;

			size_t valuesSize = sizeof(uint64_t) * numInstances;

			// iterate all events
			for(uint32_t j = 0; j < numEvents; j++) {
				// read event
				CHECK(cuptiEventGroupReadEvent(group, CUPTI_EVENT_READ_FLAG_NONE, eventIds[j], &valuesSize, values));

				// get id
				const uint32_t id = data.cuptiNextEventId();
				assert(id < data.cuptiEventCount());

				// sum
				sum = 0;
				for(uint32_t k = 0; k < numInstances; k++)
					sum += values[k];

				// normalize
				normalized = (sum * numTotalInstances) / numInstances;
		
				// add to CUPTIMetric
				data.cuptiEventIdArray()	[id] = eventIds[j];
				data.cuptiEventValueArray()	[id] = normalized;
			}

			// free buffer
			FREE_A(eventIds);
			FREE_A(values);
		}

		// disable event groups
		for(uint32_t i = 0; i < eventGroupSet->numEventGroups; i++) {
			CHECK(cuptiEventGroupDisable(eventGroupSet->eventGroups[i]));
		}
	}
}
	
//-------------------------------------------------------------------
CUPTI::CUPTI(CUcontext context, CUdevice deviceId) :
	m_cuptiSets				(0),
	m_cuptiCounter			(0),
	m_cuptiEventIdArray		(0),
	m_cuptiEventValueArray	(0),
	m_cuptiEventCnt			(0),
	m_cuptiEventArraySize	(0),
	m_cuptiEventId			(0),
	m_duration				(0),
	m_context				(context),
	m_deviceId				(deviceId)
{
	initMetrics();
}
 
//-------------------------------------------------------------------
CUPTI::~CUPTI(void) {
	if(m_cuptiEventIdArray)
		FREE_A(m_cuptiEventIdArray);
	if(m_cuptiEventValueArray)
		FREE_A(m_cuptiEventValueArray);
}

//-------------------------------------------------------------------
void CUPTI::initMetrics() {
	if(Profiler::getMetricCount() > 0) {
		const uint32_t metricCount	= Profiler::getMetricCount();
		CUpti_MetricID* metricIds	= Profiler::getMetricIds(getDeviceId());

		// create event groups
		CHECK(cuptiMetricCreateEventGroupSets(
			getContext(), 
			sizeof(CUpti_MetricID) * metricCount, 
			metricIds, 
			&m_cuptiSets)
		);
		
		// calc eventCount
		for(uint32_t s = 0; s < m_cuptiSets->numSets; s++) {
			for(uint32_t g = 0; g < m_cuptiSets->sets[s].numEventGroups; g++) {
				uint32_t numEvents = 0;
				size_t numEventsSize = sizeof(numEvents);
				CHECK(cuptiEventGroupGetAttribute(m_cuptiSets->sets[s].eventGroups[g], CUPTI_EVENT_GROUP_ATTR_NUM_EVENTS, &numEventsSize, &numEvents));

				m_cuptiEventCnt += numEvents;
			}
		}

		// reset memory
		if(m_cuptiEventCnt > m_cuptiEventArraySize) {
			if(m_cuptiEventIdArray)
				FREE_A(m_cuptiEventIdArray);
			if(m_cuptiEventValueArray)
				FREE_A(m_cuptiEventValueArray);

			m_cuptiEventArraySize	= m_cuptiEventCnt;

			m_cuptiEventIdArray		= NEW_A(CUpti_EventID, m_cuptiEventArraySize);
			m_cuptiEventValueArray	= NEW_A(uint64_t, m_cuptiEventArraySize);
		}
	}
}

//-------------------------------------------------------------------
void CUPTI::reset(void) {
	// reset counter
	m_cuptiEventId	= 0;
	m_cuptiCounter	= 0;
}

//-------------------------------------------------------------------
void CUPTI::resetForRepeat(const uint32_t runId) {
	m_cuptiEventId = 0;
	m_cuptiCounter = 0;
	event::dbDeleteMetrics(runId);
}

//-------------------------------------------------------------------
				}
			}
		}
	}
}