// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_COMPILER
#define __MATOG_RUNTIME_INTERN_EXEC_COMPILER

#include <matog/runtime.h>
#include <matog/cuda/jit/Target.h>
#include <set>
#include <mutex>
#include <tbb/task_group.h>
#include <tbb/enumerable_thread_specific.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
class Compiler {
	struct Files {
		char* log;
		char* cubin;
		char* matog;

		Files(void);
		~Files(void);
	};

	std::set<cuda::jit::Target>				m_currentlyCompiling;
	std::mutex								m_mutex;
	tbb::task_group							m_group;
	tbb::enumerable_thread_specific<Files>	m_local;

	void				run		(const char* tmpFolder, const int len);
	cuda::jit::Target	pop		(void);

public:
	Compiler			(void);
	Compiler			(const Compiler&) = delete;
	~Compiler			(void);
	bool	push		(const Exec* executor, const variant::Hash& hash);
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif