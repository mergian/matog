// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Compiler.h"
#include <matog/cuda.h>
#include <matog/shared.h>
#include <matog/util.h>
#include <matog/log.h>
#include <thread>
#include <MatogMacros.h>

namespace matog {
	namespace learn {
//-------------------------------------------------------------------
Compiler::Compiler(Queue& targets) : m_queue(targets) {
	start();
	stop();
}

//-------------------------------------------------------------------
void Compiler::stop(void) {
	join();

	if(m_threads)
		FREE_A(m_threads);
}

//-------------------------------------------------------------------
void Compiler::start(void) {
	uint32_t cnt = Config::get<uint32_t>(Config::COMPILE_THREAD_COUNT);
	
	if(cnt == 0)
		cnt = std::thread::hardware_concurrency();
	
	m_threads = NEW_A(std::thread, cnt);

	const char* tmpFolder = IO::getTmpFolder();
	const int len = (int)strlen(tmpFolder); 

	for(uint32_t i = 0; i < cnt; i++)
		m_threads[i] = std::thread(&Compiler::run, this, tmpFolder, len);
}
//-------------------------------------------------------------------
JitTarget Compiler::pop(void) {
	LOCK(m_mutex);

	if(m_queue.empty())
		return JitTarget();

	JitTarget target = *m_queue.begin();
	m_queue.erase(m_queue.begin());

	uint64_t cnt = m_queue.size();

	if(cnt % 10 == 0)
		L_TRACE("%llu...", (unsigned long long int)cnt);

	return target;
}

//-------------------------------------------------------------------
void Compiler::run(const char* tmpFolder, const int len) {
	// build tmp file strings
	char* logFile = 0, *cubinFile = 0, *matogFile = 0;
	Jit::generateFileNames(logFile, cubinFile, matogFile, tmpFolder, len);
	
	// iterate
	while(true) {
		JitTarget t = pop();
			
		if(t.getMatogCode()) {
			// init code ptr
			const Blob* code = 0;
			
			// check if this target was not compiled in the meantime!
			if(!CUBINStore::doesExist(t)) {
				// compile using JIT compiler
				Jit::compile(t, code, logFile, cubinFile, matogFile);
			
				// do we have code?
				THROWIF(code == 0 || code->getSize() == 0,"CUBINSTORE_NO_CUBIN_AVAILABLE");
			}

			if(code) {
				// save to store
				CUBINStore::add(t, code);

				// free
				FREE(code);
			}

			FREE_A(t.getMatogCode());
		} else {
			break;
		}
	}

	if(logFile)		
		delete[] logFile;
}

//-------------------------------------------------------------------
void Compiler::join(void) {
	// join all threads
	uint32_t cnt = Config::get<uint32_t>(Config::COMPILE_THREAD_COUNT);
	if(cnt == 0)
		cnt = std::thread::hardware_concurrency();

	for(uint32_t i = 0; i < cnt; i++) {
		if(m_threads[i].joinable())
			m_threads[i].join();
	}
}

//-------------------------------------------------------------------
	}
}