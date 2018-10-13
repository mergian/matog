// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Optimized.h"
#include <matog/runtime/intern/DeviceArray.h>
#include <matog/runtime/intern/Predictor.h>
#include <matog/runtime/intern/Decisions.h>
#include <matog/runtime/intern/Runtime.h>
#include <matog/runtime/intern/context/cache/Item.h>

#include <matog/degree/Type.h>

#include <matog/Variant.h>

#include <matog/log.h>
#include <matog/macros.h>

using namespace matog::cuda;

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
using namespace matog::variant;

bool Optimized::s_isLogEnabled = false;

//-------------------------------------------------------------------
bool Optimized::isLogEnabled(void) {
	return s_isLogEnabled;
}

//-------------------------------------------------------------------
void Optimized::setLogEnabled(const bool value) {
	s_isLogEnabled = value;
}

//-------------------------------------------------------------------
Optimized::Optimized(Params& params) :
	Exec			(params),
	m_isCompiled	(false)
{}

//-------------------------------------------------------------------
Optimized::~Optimized(void) {
}

//-------------------------------------------------------------------
void Optimized::notify(const jit::Target& target) {
	{
		ULOCK(m_mutex);
		m_isCompiled = true;
		m_jit = target;
	}

	m_cv.notify_all();
}

//-------------------------------------------------------------------
void Optimized::wait(void) {
	ULOCK(m_mutex);
	while(!m_isCompiled) m_cv.wait(lock);
}

//-------------------------------------------------------------------
Hash Optimized::initNonInitializedArrays(void) {
	Hash globalHash = 0;

	const Variant& kVariant = getKernel().getVariant();

	// init decision ids and register to ctx
	for(auto& item : getDeviceArrays()) {
		DeviceArray* ptr = item.second;

		// only non initialized
		if(!ptr->isInitialized() || getKernel().getRWMode(ptr->getArray().getId(), ptr->getParamId()) == RWMode::WRITE_ONLY)
			getContext().addDecision((intern::MemArray<>*)ptr, &getKernel(), &getGPU());
	}

	// set configs and calc globalHash
	for(auto& item : getDeviceArrays()) {
		DeviceArray* ptr = item.second;
		
		// get array
		const Array& array		= ptr->getArray();
		const Variant& aVariant	= array.getVariant();
		const uint32_t paramId	= ptr->getParamId();

		// only non initialized
		if(!ptr->isInitialized() || getKernel().getRWMode(ptr->getArray().getId(), ptr->getParamId()) == RWMode::WRITE_ONLY) {
			ptr->setConfig(aVariant.uncompress(Predictor::predict(ptr->getDecisionId(), &getParams()), Type::Global));
		}
		
		// store config
		for(const Item& item : array.getItems()) {
			const Degree* degree = item.getDegree();

			if(degree->isGlobal())
				globalHash += kVariant.hash(degree, paramId, aVariant.get(ptr->getConfig(), item));
		}
	}

	return globalHash;
}

//-------------------------------------------------------------------
void Optimized::run(void) {
	const uint32_t decisionId = Decisions::getId(0, &getKernel(), &getGPU());

	// init non initialized arrays and store their config in m_config
	const Hash globalHash = initNonInitializedArrays();
	
	// predict local hash
	const Hash localHash = getKernel().getVariant().uncompress(Predictor::predict(decisionId, &getParams()), Type::Local);

	// calc hash
	const Hash finalHash = globalHash + localHash;

	// debug print
	L_DEBUG_
	log::setColor(log::TEAL);
	printf("%30s", getKernel().getCompleteName());
	log::resetColor();
	printf(":");
	getKernel().getVariant().print(finalHash);
	L_END

	// compile
	if(compile(finalHash)) {
		L_WARN("Have to compile module %s in config 0x%016llX", getModule().getName(), (unsigned long long int)finalHash);
		wait();
	} else {
		const Hash moduleHash = getKernel().getVariant().convert(finalHash, Type::Module);
		m_jit = jit::Target(getModule().getId(), getGPU().getCC(), moduleHash, getModule().getSourceFile(), getUserHash(), getUserFlags());
	}

	// get module and function
	setLaunch(getCache().get(m_jit), finalHash);

	// launch kernel
	CHECK(launchKernel());
}

//-------------------------------------------------------------------
			}
		}
	}
}