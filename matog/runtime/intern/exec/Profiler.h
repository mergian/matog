// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_PROFILER
#define __MATOG_RUNTIME_INTERN_EXEC_PROFILER

#include <matog/runtime.h>
#include <matog/runtime/intern/Exec.h>
#include <matog/runtime/intern/exec/profiler/CUPTI.h>
#include <matog/runtime/intern/exec/profiler/AllocStore.h>
#include <matog/runtime/intern/exec/profiler/ArrayStore.h>
#include <matog/runtime/intern/exec/profiler/Converter.h>
#include <matog/cuda/Occupancy.h>
#include <matog/kernel/Meta.h>
#include <map>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
using variant::Hash;
using variant::Item;

//-------------------------------------------------------------------
class Profiler : public Exec {
//-------------------------------------------------------------------
// STATIC
//-------------------------------------------------------------------
private:
	static struct Instances {
		std::map <uint32_t, profiler::CUPTI*>	map;
		std::mutex								mutex;
	} s_instances;
	
	static event::ProfilingAlgorithm							s_algorithm;
	static uint32_t												s_profilingId;
	static std::map<const Kernel*, uint32_t>					s_kernelRunCounts;
	static std::map<CUdevice, std::vector<CUpti_MetricID> >		s_metricIds;
	static uint32_t												s_metricCount;
	static profiler::Converter									s_converter;
	
	static			void						initCupti			(void);
	static			void						initMetrics			(void);
	static			event::ProfilingAlgorithm	getAlgorithm		(void);
	
public:
	static			void				init			(void);
	static			void				finalize		(void);
	static	inline	uint32_t			getId			(void) { return s_profilingId; }
	static			void				notifyAll		(const uint32_t compilerHash);
	static			uint32_t			getMetricCount	(void);
	static			CUpti_MetricID*		getMetricIds	(CUdevice device);

	static			Profiler*			getProfiler		(Params&);

	static			void				addCuptiInstance(const uint32_t id, profiler::CUPTI* data);
	static			void				setCuptiValues	(const uint32_t id, const uint64_t duration, const L1Cache requested, const L1Cache executed);
			
//-------------------------------------------------------------------
// INSTANCE
//-------------------------------------------------------------------
private:
	// mutex
	std::mutex								m_mutex;
	std::condition_variable					m_cv;

	// profiler
	profiler::AllocStore					m_allocStore;
	profiler::ArrayStore					m_arrayStore;
	const Variant&							m_config;
	Hash									m_currentModuleHash;
	Hash									m_previousModuleHash;
	cuda::Occupancy							m_occupancy;
	profiler::CUPTI							m_cuptiData;

	// profiling
	uint32_t								m_rerunCount;
	uint32_t								m_callId;
	uint32_t								m_runId;
	uint32_t								m_kernelRunIndex;
	bool									m_runAtLeastOnce;
	bool									m_verify;

	// targets
	variant::Hash							m_bestTarget;
	uint64_t								m_bestDuration;
	uint32_t								m_waitCount;

	std::queue<Hash>						m_queue;
	std::map<Hash, std::set<Hash> >			m_waiting;
	std::map<Hash, uint32_t>				m_runIds;
	std::map<Hash, uint32_t>				m_notificationCount;
	std::map<uint32_t, intern::HostArray*>	m_verifyArrays;
	std::map<CUdeviceptr, char*>			m_verifyAllocs;
	std::set<Item>							m_nonTextureArrays;
	
	typedef std::map<Hash, uint64_t>		DurationMap;
	DurationMap m_duration;
	
	// init functions
	void		initKernelRunIndex						(void);
	void		initCall								(void);
	void		initNonTextureArrays					(void);

	// run helper functions
	void					prepareData					(void);

	void					runTiming					(void);
	void					runMetric					(void);
	virtual void			notify						(const cuda::jit::Target& target);
	bool					popNext						(void);
	bool					decreaseNotificationCount	(const Hash hash);
	void					verify						(void);
	void					prepareRun					(void);
	void					prepareModule				(void);
	bool					calcMetrics					(void);

	inline	uint64_t		getDuration					(void) const { return m_cuptiData.getDuration(); }
	inline	L1Cache			getExecutedCache			(void) const { return m_cuptiData.getExecutedCache(); }
	inline	L1Cache			getRequestedCache			(void) const { return m_cuptiData.getRequestedCache(); }

protected:
	bool										validate					(const Hash hash);
												Profiler					(Params& params);
	bool										enqueue						(const Hash hash);
	void										runQueue					(void);
	void										skipKernel					(void);
	
			uint64_t							getDuration					(const Hash hash) const;
	inline	uint32_t							getCallId					(void) const { return m_callId; }
	inline	uint32_t							getRunId					(void) const { return m_runId; }
	inline	uint32_t							getKernelRunIndex			(void) const { return m_kernelRunIndex; }
	inline	bool								isRunAtLeastOnce			(void) const { return m_runAtLeastOnce; }
			uint32_t							getRunId					(const Hash hash) const;
	inline	Hash								getBestTarget				(void) { return m_bestTarget; }
	inline	uint64_t							getBestDuration				(void) { return m_bestDuration; }
	inline	uint32_t							getExecutedTargetCount		(void) const { return (uint32_t)m_duration.size(); }
	inline	const DurationMap&					getAllDurations				(void) const { return m_duration; }
	inline	const std::set<Item>&				getNonTextureArrays			(void) const { return m_nonTextureArrays; }
		
	// virtual
	virtual	bool								wasTargetAlreadyExecuted	(const Hash hash);
	virtual	void								preTimingRun				(void);
	virtual void								postTimingRun				(void);
	virtual	void								preMetricRun				(void);
	virtual	void								postMetricRun				(void);
	virtual void								postProfiling				(void);

public:
	// instance
	virtual										~Profiler			(void);
	void										postRun				(void);
	void										notifyConversion	(const Hash hash);

	// abstract instance
	virtual	void								run					(void) = 0;

	friend class profiler::ArrayStore;
	friend class profiler::Converter;
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif