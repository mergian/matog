// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Profiler.h"
#include "profiler/CUPTI.h"
#include <matog/runtime/intern/HostArray.h>
#include <matog/runtime/intern/DeviceArray.h>
#include <matog/runtime/intern/DynArray.h>

#include <matog/cuda/Architecture.h>
#include <matog/cuda/jit/Target.h>

#include <matog/Options.h>
#include <matog/degree/Type.h>
#include <matog/kernel/Meta.h>
#include <matog/event/Type.h>

#include <matog/log.h>
#include <matog/macros.h>

using namespace matog::cuda;
using namespace matog::variant;
using namespace matog::degree;
using namespace matog::kernel;

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
bool Profiler::calcMetrics(void) {
	if(m_cuptiData.getRunCount() > 0) {
		// iterate metrics
		assert(m_cuptiData.cuptiEventCount());
		assert(m_cuptiData.cuptiEventIdArray());
		assert(m_cuptiData.cuptiEventValueArray());
		assert(m_cuptiData.getDuration());
		assert(getMetricCount());
		assert(getMetricIds(getDeviceId()));

		CUpti_MetricID* ids = getMetricIds(getDeviceId());

		for(uint32_t i = 0; i < getMetricCount(); i++) {
			// calc metric
			CUpti_MetricValue value;
			CUptiResult res = cuptiMetricGetValue(getDeviceId(), ids[i], 
				m_cuptiData.cuptiEventCount() * sizeof(CUpti_EventID),	m_cuptiData.cuptiEventIdArray(), 
				m_cuptiData.cuptiEventCount() * sizeof(uint64_t),		m_cuptiData.cuptiEventValueArray(),
				m_cuptiData.getDuration(), &value);

			if(res == CUPTI_ERROR_INVALID_METRIC_VALUE) {
				L_ERROR("CUPTI_ERROR_INVALID_METRIC_VALUE: %u", ids[i]);
				return false;
			}

			CHECK(res);
					                                     
			// get kind
			CUpti_MetricValueKind kind;
			size_t kindSize = sizeof(kind);
			CHECK(cuptiMetricGetAttribute(ids[i], CUPTI_METRIC_ATTR_VALUE_KIND, &kindSize, &kind));

			// add to metric
			event::dbInsertMetric(getRunId(), ids[i], value, kind);
		}
	}

	return true;
}

//-------------------------------------------------------------------
Profiler::Profiler(Params& params) : 
	Exec					(params),
	m_allocStore			(getParams()),
	m_arrayStore			(getDeviceArrays(), getParams(), getKernel()),
	m_config				(getKernel().getVariant()),
	m_currentModuleHash		(0),
	m_previousModuleHash	(0),
	m_occupancy				(getThreadCount(), getGPU()),
	m_cuptiData				(getContext().getContext(), getDeviceId()),
	m_rerunCount			(Options::get<uint32_t>({"profiling","rerun_count"}, 1)),
	m_callId				(0),
	m_runId					(0),
	m_kernelRunIndex		(0),
	m_runAtLeastOnce		(false),
	m_verify				(Options::get<bool>({"profiling", "verify"}, false)),
	m_bestTarget			(0),
	m_bestDuration			(UINT64_MAX),
	m_waitCount				(0)
{
	// init run index
	initKernelRunIndex();

	// init db call
	initCall();

	// init non texture arrays for validation
	initNonTextureArrays();
		
	// print
	C_INFO(log::GREEN, "### %s::%s // %u", getModule().getName(), getKernel().getName(), getKernelRunIndex());
}

//-------------------------------------------------------------------
void Profiler::initNonTextureArrays(void) {
	for(const auto& item : getDeviceArrays()) {
		const DeviceArray* dev = item.second;
		const Array& array = dev->getArray();

		// has memory degree and isTexture?
		if(!isTexture(dev))
			m_nonTextureArrays.emplace(array.getRootMemory(), dev->getParamId());
	}
}

//-------------------------------------------------------------------
bool Profiler::wasTargetAlreadyExecuted(const Hash hash) { 
	return m_duration.find(hash) != m_duration.end(); 
}

//-------------------------------------------------------------------
void Profiler::initKernelRunIndex(void) {
	// get entry in map
	auto it = s_kernelRunCounts.find(&getKernel());

	// create new entry if does not exist
	if(it == s_kernelRunCounts.end())
		it = s_kernelRunCounts.emplace(MAP_EMPLACE(&getKernel(), 0)).first;

	// get kernel run index
	m_kernelRunIndex = it->second++;
}

//-------------------------------------------------------------------
void Profiler::initCall(void) {
	assert(m_callId == 0);
	
	// init all non-initialized and write_only device arrays
	std::list<DeviceArray*> init;
	Context& ctx = getContext();

	for(const auto& it : getDeviceArrays()) {
		auto ptr = it.second;

		// if array is write only or uninitialized, then create new decision event
		if(getKernel().getRWMode(ptr->getArray().getId(), ptr->getParamId()) == RWMode::WRITE_ONLY || !it.second->isInitialized()) {
			// init decision
			ctx.addDecision((MemArray<>*)it.second, &getKernel(), &getGPU());

			// add event
			ptr->setEventId(event::dbInsertEvent(exec::Profiler::getId(), ptr->getDecisionId()));

			// schedule to store meta data
			// we do this after this loop, so all arrays have the most recent information.
			init.emplace_back(it.second);
		}
	}

	// insert array meta for all newly inited arrays
	for(DeviceArray* ptr : init) {
		ctx.dbInsertArrayMeta(ptr->getEventId());
		ctx.dbInsertUserMeta (ptr->getEventId());
	}

	// insert launch configuration
	m_callId = event::dbInsertCall(getId(), getParams().decisionId, getGPU().getId(), getKernel().getId(), getUserHash(), getParams().grid, getParams().block, getParams().sharedMemBytes);
	
	// insert array meta data to call
	for(const auto& it : getDeviceArrays())
		getContext().dbInsertCallMeta(m_callId, it.second);
		
	// insert data to call
	getContext().dbInsertUserMeta(m_callId);
}

//-------------------------------------------------------------------
Profiler::~Profiler(void) {
	for(auto& item : m_verifyArrays)
		delete item.second;

	for(auto& item : m_verifyAllocs)
		FREE_A(item.second);
}

//-------------------------------------------------------------------
void Profiler::postRun(void) {
	// was this kernel run at least once?
	if(!m_runAtLeastOnce) {
		// if not, we have to run at least the baseline, so that the data is in a proper state
		L_WARN("### running baseline as no other runs have been performed!");

		enqueue(0); // 0 is baseline by definition
		runQueue();
	} 
	// do not print, if "runatleastonce", because TARGET exists only in local scope!!!
	else {
		C_INFO(log::GREEN, "### >> %.2fms (%u runs)", m_bestDuration/1000000.0f, (int)m_duration.size());
	}

	// run callback
	postProfiling();
}

//-------------------------------------------------------------------
uint32_t Profiler::getRunId(const Hash hash) const {
	auto it = m_runIds.find(hash);

	if(it == m_runIds.end())
		return 0;

	return it->second;
}

//-------------------------------------------------------------------
void Profiler::prepareData(void) {
	// Restore Allocations
	m_allocStore.restore();

	// DEVICE ARRAYS
	for(const auto& item : getDeviceArrays()) {
		const uint32_t index	= item.first;
		DeviceArray* ptr		= item.second;
		const Array& array		= ptr->getArray();
		const Variant& aVariant	= array.getVariant();

		// get instance id
		const uint32_t paramId = ptr->getParamId();

		// calc configs
		variant::Hash arrayHash = 0;

		for(const auto& it : aVariant.subset(variant::Type::Global)) 
			arrayHash += aVariant.hash(it, m_config.get(getLaunchHash(), it.getDegree(), paramId));

		// get instance and copy data to device
		m_arrayStore.getDeviceArray(index, arrayHash);
	}

	// sync
	CHECK(::cuStreamSynchronize(getParams().stream));
}

//-------------------------------------------------------------------
void Profiler::skipKernel(void) {
	setLaunchHash(UINT64_MAX);
	event::dbDeleteRun(m_runId);
}

//-------------------------------------------------------------------
bool Profiler::validate(const Hash hash) {
	const Kernel& kernel = getKernel();

	// Check min/max arch
	const GPU& gpu = getGPU();
	const auto arch = gpu.getArchitecture();
	
	for(const auto& it : kernel.getDegrees()) {
		const Degree* d = it.getDegree();
		
		if(d->getType() == degree::Type::L1Cache) {
			const variant::Value value = m_config.get(hash, it);
			
			if(arch < d->getMinArch(value) || arch > d->getMaxArch(value))
				return false;
		}
	}

	// Check if Variable is READ_ONLY
	for(const auto& it : m_nonTextureArrays) {
		const Memory value = ((Memory)m_config.get(hash, it));

		if(value == Memory::TEXTURE)
			return false;
	}

	// all tests passed
	return true;
}

//-------------------------------------------------------------------
bool Profiler::enqueue(const Hash hash) {
	// was already executed?
	if(wasTargetAlreadyExecuted(hash))
		return false;

	// is target valid for the given kernel call?
	if(!validate(hash))
		return false;

	{ // is already enqueued?
		ULOCK(m_mutex);
		if(m_notificationCount.find(hash) != m_notificationCount.end())
			return false;
	}

	ULOCK(m_mutex);
	uint32_t notificationCount = 0;

	// schedule for conversion
	m_arrayStore.convertArray(notificationCount, this, getKernel().getVariant(), hash, getModule().getArrays());
	
	// schedule for compilation
	if(compile(hash)) 
		notificationCount++;
	
	// READY FOR EXECUTION
	if(notificationCount == 0) {
		m_queue.emplace(hash);
	} else {
		m_waiting[m_config.convert(hash, variant::Type::Module)].emplace(hash);
		m_notificationCount[hash] = notificationCount;
		m_waitCount++;
	}
	
	return true;
}

//-------------------------------------------------------------------
bool Profiler::popNext(void) {
	ULOCK(m_mutex);

	// return false if not waiting an queue empty
	if(m_queue.empty() && m_waitCount == 0)
		return false;

	// wait if m_waiting is not zero but m_queue.empty()
	while(m_waitCount && m_queue.empty()) m_cv.wait(lock);

	setLaunchHash(m_queue.front());
	m_queue.pop();

	m_currentModuleHash = m_config.convert(getLaunchHash(), variant::Type::Module);

	return true;
}

//-------------------------------------------------------------------
void Profiler::prepareModule(void) {
	if (getLaunchItem() == 0 || m_previousModuleHash != m_currentModuleHash) {
		m_previousModuleHash = m_currentModuleHash;

		// construct JIT
		jit::Target jit(
			getModule().getId(),
			getGPU().getCC(),
			m_currentModuleHash,
			getModule().getSourceFile(),
			getUserHash(),
			getUserFlags()
		);

		// load new module
		setLaunchItem(getCache().get(jit));
	}
}

//-------------------------------------------------------------------
void Profiler::prepareRun(void) {
	// set run at least once
	if(!m_runAtLeastOnce)
		m_runAtLeastOnce = true;

	// register kernel run
	m_runId = event::dbInsertRun(getCallId(), getLaunchHash());
		
	// register runId
	m_runIds.emplace(MAP_EMPLACE(getLaunchHash(), m_runId));
}

//-------------------------------------------------------------------
void Profiler::runQueue(void) {
	// run until queue is empty
	while(popNext()) {
		// skip target
		if(m_duration.find(getLaunchHash()) != m_duration.end())
			continue;
		
		// prepare
		prepareModule();
		prepareRun();

		// run timing
		runTiming();

		// run metric if necessary
		if(getMetricCount() > 0)
			runMetric();
	}
}

//-------------------------------------------------------------------
void Profiler::runTiming(void) {
	preTimingRun();

	// get context
	CUcontext ctx = getContext().getContext();

	// Enable Callback
	CUpti_SubscriberHandle subscriber;
	CHECK(cuptiSubscribe(&subscriber, (CUpti_CallbackFunc)&profiler::CUPTI::cuptiActivityCallback, &m_cuptiData));

	// Enable Domain
	CHECK(cuptiEnableDomain(1, subscriber, CUPTI_CB_DOMAIN_DRIVER_API));
	CHECK(cuptiActivityEnableContext(ctx, CUPTI_ACTIVITY_KIND_KERNEL));
	
	// execute reruns
	uint64_t newDuration = 0;

	for(uint32_t cnt = 0; cnt < m_rerunCount; cnt++) {
		// get meta
		const kernel::Meta& meta = getLaunchItem()->getMeta(getKernel());

		// prepare data
		prepareData();

		// run kernel
		uint32_t sharedMemBytes = 0;

		CHECK(launchKernel(&sharedMemBytes));
		
		{ // store theoretical occupancy
			event::dbSetRunSharedMemory(getRunId(), meta.getStaticSharedMem(), sharedMemBytes);
			const L1Cache cache = (L1Cache)m_config.get(getLaunchHash(), &Static::getDegree(1), 0);
			const double kernelOccupancy = m_occupancy.calc(meta.getUsedRegisters(), sharedMemBytes + meta.getStaticSharedMem(), cache);
			event::dbSetRunOccupancy(getRunId(), kernelOccupancy);
		}
	
		// sync ctx
		CUresult result = cuCtxSynchronize();

		if(result != CUDA_SUCCESS) {
			C_ERROR(log::YELLOW, "failed on id %llu --> %llu", (unsigned long long int)getLaunchHash(), (unsigned long long int)m_currentModuleHash);
			L_ERROR_
			m_config.print(getLaunchHash());
			L_END
			if(log::isDebug()) {
				const char* msg = getKernel().generateMatogCode(getLaunchHash(), getGPU().getArchitecture());
				L_ERROR("%s", msg);
				FREE_A(msg);
			}
			CHECK(result);
		}

		// flush
		CHECK(cuptiActivityFlushAll(0));
		
		// sum up duration for averaging
		newDuration += getDuration();
	}

	// insert into db
	if(newDuration != UINT64_MAX) {
		newDuration /= m_rerunCount;

		if(getBestDuration() > newDuration) {
			m_bestDuration	= newDuration;
			m_bestTarget	= getLaunchHash();
		}

		if(m_verify)
			verify();

		event::dbSetRunData(getRunId(), newDuration, (uint32_t)getRequestedCache(), (uint32_t)getExecutedCache());
	}

	// print log
	if(log::isDebug()) {
		L_DEBUG_
		if(getBestDuration() == getDuration())
			log::setColor(log::Color::GREEN);

		printf("%3u / %3u // %16llX ", (uint32_t)m_queue.size(), m_waitCount, (unsigned long long int)getLaunchHash());
		printf("%.2f << %.2f ", getBestDuration()/1000.0, getDuration()/1000.0);
		m_config.print(getLaunchHash());
		L_END
	}

	m_duration.emplace(MAP_EMPLACE(getLaunchHash(), newDuration));

	// Disable Kernel Activity
	CHECK(cuptiActivityDisableContext(ctx, CUPTI_ACTIVITY_KIND_KERNEL));

	// remove subscriber
	CHECK(cuptiUnsubscribe(subscriber));

	postTimingRun();
}

//-------------------------------------------------------------------
void Profiler::verify(void) {
	const bool firstVerify = m_verifyArrays.empty();

	// MATOG ARRAYS
	for(auto& item : getDeviceArrays()) {
		const uint32_t index	= item.first;
		DeviceArray* ptr		= item.second;

		assert(ptr->getConfig() != UINT_MAX);

		// only WRITE variables
		if(getKernel().getRWMode(ptr->getArray().getId(), ptr->getParamId()) != RWMode::READ_ONLY) {
			// copy to host
			HostArray* host = (HostArray*)ptr->clone_host("MATOG_PROFILER", 0);
			host->setConfig(ptr->getConfig());
			CHECK(cuMemcpyDtoH(host->data_ptr(), ptr->data_ptr(), host->copy_size()));
				
			// add to map if first run
			if(firstVerify) {
				m_verifyArrays.emplace(MAP_EMPLACE(index, host));
			} 

			// verify if not first run
			else {
				auto it = m_verifyArrays.find(index);

				THROWIF(it == m_verifyArrays.end(), "UNKNOWN_VALUE");
				L_INSANE_
					printf("comparing:");
					host->getArray().getVariant().print(host->getConfig());
					printf(" -->");
					it->second->getArray().getVariant().print(it->second->getConfig());
				L_END

				if(!it->second->compare(host)) {
					L_ERROR("verify failed for variable %u:%s --> %s (%u) in config %0llX", index, ((intern::BaseArray<>*)host)->getArray().getName(), ptr->file(), ptr->line(), (unsigned long long int)getLaunchHash());
					L_ERROR_
					m_config.print(getLaunchHash());
					L_END
					THROW("DATA_MISMATCH");
				}
					
				// free
				delete host;
			}
		}
	}

	// NORMAL ALLOCATIONS
	// TODO: solved very badly!
	for(auto& item : m_allocStore.m_list) {
		char* host = NEW_A(char, item.bytes);
		CHECK(cuMemcpyDtoH(host, item.device, item.bytes));

		if(firstVerify) {
			m_verifyAllocs.emplace(MAP_EMPLACE(item.device, host));
		} else {
			auto it = m_verifyAllocs.find(item.device);

			THROWIF(it == m_verifyAllocs.end(), "UNKNOWN_VALUE");

			char* other = it->second;

			for(uint64_t i = 0; i < item.bytes; i++) {
				if(host[i] != other[i]) {
					L_ERROR_
					m_config.print(getLaunchHash());
					L_END
					THROW("DATA_MISMATCH");
				}
			}

			FREE_A(host);
		}
	}
}

//-------------------------------------------------------------------
void Profiler::runMetric(void) {
	preMetricRun();

	// run count
	const uint32_t runCount = m_cuptiData.getRunCount();

	// skip if there are no metrics present
	if(runCount == 0)
		return;

	// enable subscriber in first run
	CUpti_SubscriberHandle subscriber;
	CHECK(cuptiSubscribe(&subscriber, (CUpti_CallbackFunc)&profiler::CUPTI::cuptiMetricCallback, &m_cuptiData));
	CHECK(cuptiEnableCallback(1, subscriber, CUPTI_CB_DOMAIN_DRIVER_API, CUPTI_DRIVER_TRACE_CBID_cuLaunchKernel));

	// reset cupti data
	m_cuptiData.reset();
	
	// repeat if CUPTI fails
	bool repeat = false;

	do {
		// iterate
		for(uint32_t run = 0; run < runCount; run++) {
			// prepare data
			prepareData();

			// run kernel
 			CHECK(launchKernel());

			// sync ctx
			CHECK(cuCtxSynchronize());

			// increase eventset counter
			m_cuptiData.incCuptiCounter();	
		}

		// calc metric + repeat if necessary
		if(!calcMetrics()) {
			L_WARN("repeating as CUPTI has failed to provide valid metrics");
			m_cuptiData.resetForRepeat(getRunId());
			repeat = true;
		}
	} while(repeat);

	// unsubscribe
	CHECK(cuptiUnsubscribe(subscriber));
	postMetricRun();
}

//-------------------------------------------------------------------
uint64_t Profiler::getDuration(const Hash hash) const {
	auto it = m_duration.find(hash);

	if(it != m_duration.end())
		return it->second;

	return UINT64_MAX;
}

//-------------------------------------------------------------------
void Profiler::notify(const jit::Target& target) {
	ULOCK(m_mutex);
	auto it = m_waiting.find(target.getModuleHash());

	std::list<Hash> erase;
	
	if(it != m_waiting.end()) {
		// push all entries to the execution queue
		for(const auto& hash : it->second) {
			if(decreaseNotificationCount(hash))
				erase.emplace_back(hash);
		}
		
		// remove hashs from set if count is zero
		for(const auto& hash : erase)
			it->second.erase(hash);

		// remove set from waiting if empty
		if(it->second.empty())
			m_waiting.erase(it);
	}
	
	// notify
	m_cv.notify_all();
}

//-------------------------------------------------------------------
void Profiler::notifyConversion(const Hash hash) {
	ULOCK(m_mutex);

	// decrease notification count
	if(decreaseNotificationCount(hash)) {
		auto it = m_waiting.find(m_config.convert(hash, variant::Type::Module));
		
		if(it != m_waiting.end()) {
			it->second.erase(hash);
			
			if(it->second.empty())
				m_waiting.erase(it);
		}
	}
	
	m_cv.notify_all();
}

//-------------------------------------------------------------------
bool Profiler::decreaseNotificationCount(const Hash hash) {
	auto it = m_notificationCount.find(hash);
	
	if(it != m_notificationCount.end()) {
		const uint32_t cnt = --(it->second);
		
		if(cnt == 0) {
			// erase hash from maps
			m_notificationCount.erase(it);

			// enqueue
			m_queue.emplace(hash);
			m_waitCount--;
			
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------
void Profiler::preTimingRun(void) {
}

//-------------------------------------------------------------------
void Profiler::postTimingRun(void) {
}

//-------------------------------------------------------------------
void Profiler::preMetricRun(void) {
}

//-------------------------------------------------------------------
void Profiler::postMetricRun(void) {
}

//-------------------------------------------------------------------
void Profiler::postProfiling(void) {
}

//-------------------------------------------------------------------
			}
		}
	}
}