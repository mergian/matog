// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_OPTIMIZED
#define __MATOG_RUNTIME_INTERN_EXEC_OPTIMIZED

#include <matog/runtime.h>
#include <matog/runtime/intern/Exec.h>
#include <mutex>
#include <condition_variable>
#include <matog/cuda/jit/Target.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
class Optimized : public Exec {
private:
	static bool				s_isLogEnabled;
	std::mutex				m_mutex;
	std::condition_variable	m_cv;
	cuda::jit::Target		m_jit;
	bool					m_isCompiled;

	variant::Hash initNonInitializedArrays(void);

protected:
	virtual void notify	(const cuda::jit::Target& target);
			void wait	(void);

public:
	Optimized(Params& params);
	virtual ~Optimized(void);
	void run(void);
	
	static bool isLogEnabled(void);
	static void setLogEnabled(const bool value);
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif