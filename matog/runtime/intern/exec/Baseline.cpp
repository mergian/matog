// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Baseline.h"
#include <matog/macros.h>
#include <matog/log.h>
#include <matog/degree/Type.h>
#include "Mode.h"

using namespace matog::cuda;

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
Baseline::Baseline(Params& params, const Mode mode) :
	Exec			(params),
	m_mode			(mode),
	m_compilerHash	(0),
	m_isCompiled	(false)
{}

//-------------------------------------------------------------------
Baseline::~Baseline(void) {
}

//-------------------------------------------------------------------
void Baseline::notify(const jit::Target& target) {
	{
		ULOCK(m_mutex);
		m_isCompiled = true;
		m_jit = target;
	}

	m_cv.notify_all();
}

//-------------------------------------------------------------------
void Baseline::wait(void) {
	ULOCK(m_mutex);
	while(!m_isCompiled) m_cv.wait(lock);
}

//-------------------------------------------------------------------
void Baseline::run(void) {
	variant::Hash hash = 0;

	if(m_mode != Mode::Baseline_SOA) {
		const Variant& variant = getKernel().getVariant();
		const bool isAOSOA = m_mode == exec::Mode::Baseline_AOSOA;

		for (const auto& it : variant.items()) {
			const Degree* d = it.getDegree();

			if (d->getType() == degree::Type::ArrayLayout) {
				if (!isAOSOA || d->isRoot())
					hash += variant.hash(it, isAOSOA ? 2 : 1);
			}
		}
	}

	// generate ModuleTarget
	if(compile(hash)) {
		L_WARN("compiling kernel %s", getKernel().getName());
		wait();
	} else {
		m_jit = jit::Target(getModule().getId(), getGPU().getCC(), hash, getModule().getSourceFile(), getUserHash(), getUserFlags());
	}

	// load functions
	setLaunch(getCache().get(m_jit), 0);

	// launch kernel
	CHECK(launchKernel());
}

//-------------------------------------------------------------------
			}
		}
	}
}