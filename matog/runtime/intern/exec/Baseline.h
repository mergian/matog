// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_BASELINE
#define __MATOG_RUNTIME_INTERN_EXEC_BASELINE

#include <mutex>
#include <condition_variable>
#include <matog/cuda/jit/Target.h>
#include <matog/runtime.h>
#include <matog/runtime/intern/Exec.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
class Baseline : public Exec {
protected:
	const Mode				m_mode;
	uint32_t				m_compilerHash;
	bool					m_isCompiled;
	std::mutex				m_mutex;
	std::condition_variable	m_cv;
	cuda::jit::Target		m_jit;

	virtual void notify	(const cuda::jit::Target& target);
			void wait	(void);

public:
	Baseline(Params& params, const Mode mode);
	virtual ~Baseline(void);
	void run(void);
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif