// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_PROFILER_CUPTI
#define __MATOG_RUNTIME_INTERN_EXEC_PROFILER_CUPTI

#include <matog/enums.h>
#define WIN32_LEAN_AND_MEAN 1 // fix for the inconsistent definition of this macro...
#include <cupti.h>
#include <cassert>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
class CUPTI {
private:
	CUpti_EventGroupSets*			m_cuptiSets;
	uint32_t						m_cuptiCounter;
	CUpti_EventID*					m_cuptiEventIdArray;
	uint64_t*						m_cuptiEventValueArray;
	uint32_t						m_cuptiEventCnt;
	uint32_t						m_cuptiEventArraySize;
	uint32_t						m_cuptiEventId;
	uint64_t						m_duration;
	L1Cache							m_requestedCache;
	L1Cache							m_executedCache;
	CUcontext						m_context;
	CUdevice						m_deviceId;

	void							initMetrics				(void);

public:
									CUPTI					(CUcontext context, CUdevice deviceId);
	virtual							~CUPTI					(void);
	void							reset					(void);
	void							resetForRepeat			(const uint32_t runId);

	inline	void					setDuration				(const uint64_t duration)	{ m_duration = duration; }
	inline	void					setRequestedCache		(const L1Cache cache)		{ m_requestedCache = cache; }
	inline	void					setExecutedCache		(const L1Cache cache)		{ m_executedCache = cache; }
	inline	uint64_t				getDuration				(void) const				{ return m_duration; }
	inline	L1Cache					getRequestedCache		(void) const				{ return m_requestedCache; }
	inline	L1Cache					getExecutedCache		(void) const				{ return m_executedCache; }
	inline	CUpti_EventGroupSets*	cuptiSets				(void) const				{ return m_cuptiSets; }
	inline	CUpti_EventID*			cuptiEventIdArray		(void) const				{ return m_cuptiEventIdArray; }
	inline	uint64_t*				cuptiEventValueArray	(void) const				{ return m_cuptiEventValueArray; }
	inline	uint32_t				cuptiCounter			(void) const				{ return m_cuptiCounter; }
	inline	void					incCuptiCounter			(void)						{ m_cuptiCounter++; }
	inline	uint32_t				cuptiEventCount			(void) const				{ return m_cuptiEventCnt; }
	inline	uint32_t				cuptiNextEventId		(void)						{ assert(m_cuptiEventId < m_cuptiEventCnt); return m_cuptiEventId++; }
	inline	uint32_t				getRunCount				(void)						{ return (m_cuptiSets ? m_cuptiSets->numSets : 0); }
	inline	CUdevice				getDeviceId				(void)						{ return m_deviceId; }
	inline	CUcontext				getContext				(void)						{ return m_context; }

	static	void CUPTIAPI	cuptiMetricCallback						(void* userdata, CUpti_CallbackDomain domain, CUpti_CallbackId cbid, const CUpti_CallbackData* cbInfo);
	static	void CUPTIAPI	cuptiActivityBufferRequestedCallback	(uint8_t** buffer, size_t* size, size_t* maxNumRecords);
	static	void CUPTIAPI	cuptiActivityBufferCompletedCallback	(CUcontext ctx, uint32_t streamId, uint8_t* buffer, size_t size, size_t validSize);
	static	void CUPTIAPI	cuptiActivityCallback					(void* userdata, CUpti_CallbackDomain domain, CUpti_CallbackId cbid, const CUpti_CallbackData* cbdata);
};

//-------------------------------------------------------------------
				}
			}
		}
	}
}

#endif