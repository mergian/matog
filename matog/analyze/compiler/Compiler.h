// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_LEARN_COMPILER_H
#define __MATOG_LEARN_COMPILER_H

#include <matog/cuda/JitTarget.h>
#include <matog/shared/Kernel.h>
#include <set>
#include <mutex>
#include <thread>
#include <map>

namespace matog {
	namespace learn {
//-------------------------------------------------------------------
class Compiler {
	typedef std::set<cuda::JitTarget> Queue;
	Queue&						m_queue;
	std::mutex					m_mutex;
	std::thread*				m_threads;

	void				run		(const char* tmpFolder, const int len);
	cuda::JitTarget		pop		(void);

	public:
		Compiler			(Queue& targets);
		void	join		(void);
		void	start		(void);
		void	stop		(void);
};

//-------------------------------------------------------------------
	}
}

#endif