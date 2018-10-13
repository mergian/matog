// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Runtime.h"
#include "Predictor.h"
#include "Decisions.h"
#include "exec/Mode.h"
#include "exec/Baseline.h"
#include "exec/Optimized.h"
#include "exec/Profiler.h"
#include <matog/array/field/Type.h>
#include <matog/variant/Type.h>
#include <matog/runtime.h>
#include <matog/degree/Type.h>

#include <matog/DB.h>
#include <matog/db/Stmt.h>
#include <matog/db/Clear.h>
#include <matog/Options.h>
#include <matog/db/PredictionModel.h>
#include <matog/db/tables.h>

#include <matog/cuda/CUBINStore.h>

#include <matog/util/Env.h>

#include <matog/log.h>

#include <tbb/tbb.h>

using namespace matog::db;
using namespace matog::util;

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
exec::Mode Runtime::s_mode = exec::Mode::Optimized;

//-------------------------------------------------------------------
void Runtime::init(void) {
	// init options
	Options::init();

	// set db config
	DB::setFailIfDoesNotExist(true);
		
	// clear DB?
	if(Env::get("MATOG_CLEAR_DB")) {
		L_TRACE("clearing db..");
		Clear::all();
	}

	if(Env::get("MATOG_CLEAR_CUBINS")) {
		L_TRACE("clearing cubins...");
		Clear::cubins();
	}

	if(Env::get("MATOG_CLEAR_PROFILINGS")) {
		L_TRACE("clearing profiling...");
		Clear::profilings();
	}

	// get all degrees
	Static::init();

	// init decisions
	Decisions::init();
	
	// determine mode
	const char* baseline = Env::get((const char*)"MATOG_BASELINE");
	if(baseline) {
		const int mode = atoi(baseline);

		if(mode == 1)
			s_mode = exec::Mode::Baseline_AOS;
		else if(mode == 2)
			s_mode = exec::Mode::Baseline_AOSOA;
		else
			s_mode = exec::Mode::Baseline_SOA;
	} else if(Env::get((const char*)"MATOG_PROFILING"))
		s_mode = exec::Mode::Profiling;
	
	// is optimized?
	if(isOptimized()) {
		// fall back if no models available
		if(!Predictor::init())
			s_mode = exec::Mode::Baseline_SOA;
		else 		
			exec::Optimized::setLogEnabled(Options::get<bool>({"log", "optimized"}, false));
	}
	
	// init profiling
	if(isProfiling()) {
		exec::Profiler::init();
		db::PredictionModel::dbDeleteAll();
	}

	// init CUBINS
	cuda::CUBINStore::init();

	// print mode
	if(log::isDebug()) {
		switch(s_mode) {
		case exec::Mode::Optimized:
			L_DEBUG("running in OPTIMIZED MODE");
			break;
		                
		case exec::Mode::Baseline_SOA:
			L_DEBUG("running in BASELINE SOA MODE");
			break;

		case exec::Mode::Baseline_AOS:
			L_DEBUG("running in BASELINE AOS MODE");
			break;

		case exec::Mode::Baseline_AOSOA:
			L_DEBUG("running in BASELINE AOSOA MODE");
			break;
		                
		case exec::Mode::Profiling:
			L_DEBUG("running in PROFILING MODE");
			break;
		};
	}
}

//-------------------------------------------------------------------
variant::Hash Runtime::getOptimalConfig(const intern::BaseArray<>* ptr) {
	// only host vars are initialized with a layout
	if(ptr->getType() == intern::BaseArray<>::Host) {
		if(isProfiling()) {
			Context& ctx = Context::get();
			ctx.dbInsertArrayMeta(ptr->getEventId());
			ctx.dbInsertUserMeta (ptr->getEventId());
		}

		if(!isOptimized()) {
			if (s_mode == exec::Mode::Baseline_SOA)
				return 0;
			else {
				const Variant& variant = ptr->getArray().getVariant();
				const bool isAOSOA = s_mode == exec::Mode::Baseline_AOSOA;

				variant::Hash hash = 0;

				for (const auto& it : variant.items()) {
					const Degree* d = it.getDegree();

					if (d->getType() == degree::Type::ArrayLayout) {
						if (!isAOSOA || d->isRoot())
							hash += variant.hash(it, isAOSOA ? 2 : 1);
					}
				}

				return hash;
			}
		}

		return ptr->getArray().getVariant().uncompress(Predictor::predict(ptr->getDecisionId()), variant::Type::Global);
	}
	
	// device should be uninitialized
	return UINT_MAX;
}

//-------------------------------------------------------------------
void Runtime::setCompilerFlags(CUfunction f, const char* flags) {
	uint32_t kernelId = (uint32_t)(uint64_t)f;
	Context::get().setCompilerFlags(kernelId, flags);
}

//-------------------------------------------------------------------
void Runtime::registerArray(BaseArray<>* const ptr, const uint32_t arrayId, const bool isDevice) {
	// init array
	ptr->init(arrayId, isDevice);

	// register instance
	const bool isNoProfilingCopy = !isProfiling() || strcmp(ptr->file(), "MATOG_PROFILER") != 0;
	Context& ctx = Context::get();

	if(isNoProfilingCopy)
		ctx.addArray(ptr);

	// if is host
	if(ptr->getType() == intern::BaseArray<>::Host) {
		// add array to context
		if(isNoProfilingCopy && !isBaseline()) {
			ctx.addDecision((MemArray<>*)ptr);

			// insert meta data
			if(isProfiling()) {
				ptr->setEventId(event::dbInsertEvent(exec::Profiler::getId(), ptr->getDecisionId()));
				ctx.dbInsertArrayMeta(ptr->getEventId());
				ctx.dbInsertUserMeta (ptr->getEventId());
			}
		}

		// if is optimized --> predict, if baseline or profiling == 0
		if(isOptimized()) {
			((MemArray<>*)ptr)->setConfig(ptr->getArray().getVariant().uncompress(Predictor::predict(ptr->getDecisionId()), variant::Type::Global));
		} else {
			if(s_mode == exec::Mode::Baseline_SOA)
				((MemArray<>*)ptr)->setConfig(0);
			else {
				const Variant& variant = ptr->getArray().getVariant();
				const bool isAOSOA = s_mode == exec::Mode::Baseline_AOSOA;

				variant::Hash hash = 0;

				for(const auto& it : variant.items()) {
					const Degree* d = it.getDegree();
					
					if(d->getType() == degree::Type::ArrayLayout) {
						if(!isAOSOA || d->isRoot())
							hash += variant.hash(it, isAOSOA ? 2 : 1);	
					}
				}

				((MemArray<>*)ptr)->setConfig(hash);
			}
		}
	} 
	// if is device
	else if(ptr->getType() == intern::BaseArray<>::Device) {
		// device arrays must not be initialized
		((MemArray<>*)ptr)->setConfig(UINT64_MAX);
	}
}
	
//-------------------------------------------------------------------
void Runtime::deregisterArray(BaseArray<>* ptr) {
	if(!isProfiling() || strcmp(ptr->file(), "MATOG_PROFILER") != 0)
		Context::get().removeArray(ptr);
}

//-------------------------------------------------------------------
void Runtime::registerAllocation(CUdeviceptr ptr, const uint64_t bytes) { 
	Context::get().addAllocation(ptr, bytes); 
}

//-------------------------------------------------------------------
void Runtime::deregisterAllocation(CUdeviceptr ptr) { 
	Context::get().removeAllocation(ptr); 
}

//-------------------------------------------------------------------
void Runtime::watch(const int8_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, array::field::Type::S8, file, line); }
void Runtime::watch(const int16_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, array::field::Type::S16, file, line); }
void Runtime::watch(const int32_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, array::field::Type::S32, file, line); }
void Runtime::watch(const int64_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, array::field::Type::S64, file, line); }
void Runtime::watch(const uint8_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, array::field::Type::U8, file, line); }
void Runtime::watch(const uint16_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, array::field::Type::U16, file, line); }
void Runtime::watch(const uint32_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, array::field::Type::U32, file, line); }
void Runtime::watch(const uint64_t* ptr, const char* file, const int line)	{ Runtime::watch(ptr, array::field::Type::U64, file, line); }
void Runtime::watch(const float* ptr, const char* file, const int line)		{ Runtime::watch(ptr, array::field::Type::FLOAT, file, line); }
void Runtime::watch(const double* ptr, const char* file, const int line)	{ Runtime::watch(ptr, array::field::Type::DOUBLE, file, line); }

//-------------------------------------------------------------------
void Runtime::watch(const void* ptr, const array::field::Type type, const char* file, const int line) { 
	if(!isBaseline())
		Context::get().addMeta(ptr, type, file, line);	
}
	
//-------------------------------------------------------------------
void Runtime::unwatch(const void* ptr) {
	if(!isBaseline())
		Context::get().removeMeta(ptr);
}

//-------------------------------------------------------------------
bool Runtime::isProfiling(void) {
	return s_mode == exec::Mode::Profiling;
}

//-------------------------------------------------------------------
bool Runtime::isOptimized(void) { 
	return s_mode == exec::Mode::Optimized; 
}

//-------------------------------------------------------------------
bool Runtime::isBaseline(void) {
	return s_mode == exec::Mode::Baseline_SOA || s_mode == exec::Mode::Baseline_AOS || s_mode == exec::Mode::Baseline_AOSOA; 
}

//-------------------------------------------------------------------
		}
	}
}