// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Compiler.h"
#include <matog/runtime/intern/Exec.h>

#include <matog/cuda/Jit.h>
#include <matog/cuda/jit/Target.h>
#include <matog/cuda/CUBINStore.h>

#include <matog/runtime.h>
#include <matog/Options.h>

#include <matog/util/IO.h>
#include <matog/util/Mem.h>

#include <matog/log.h>
#include <matog/macros.h>

using namespace matog::util;
using namespace matog::cuda;

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
//-------------------------------------------------------------------
Compiler::Files::Files(void) : log(0), cubin(0), matog(0) {
}

//-------------------------------------------------------------------
Compiler::Files::~Files(void) {
	if(log)
		delete[] log;
}

//-------------------------------------------------------------------
Compiler::Compiler(void) {
}

//-------------------------------------------------------------------
Compiler::~Compiler(void) {
	m_group.wait();
}

//-------------------------------------------------------------------
bool Compiler::push(const Exec* exec, const variant::Hash& hash) {
	ULOCK(m_mutex);

	const Kernel& kernel	= exec->getKernel();
	const Module& module	= exec->getModule();
	const GPU& gpu			= exec->getGPU();

	jit::Target jit(
		kernel.getModuleId(),
		gpu.getCC(),
		kernel.getVariant().convert(hash, variant::Type::Module),
		module.getSourceFile(),
		exec->getUserHash(),
		exec->getUserFlags()
	);

	jit.setContext(exec->getContext().getContext());

	// check if it is already compiled
	if(CUBINStore::doesExist(jit))
		return false;

	// is this kernel already in the queue?
	auto cit = m_currentlyCompiling.find(jit);

	// not in queue?
	if(cit == m_currentlyCompiling.end()) {
		jit.setMatogCode(kernel.generateMatogCode(hash, gpu.getArchitecture()));
		m_currentlyCompiling.emplace(jit);

		m_group.run([jit, this](){
			auto& local = m_local.local();

			// init file names if necessary
			if(local.log == 0) {
				const char* tmpFolder = IO::getTmpFolder();
				const int len = (int)strlen(tmpFolder);

				Jit::generateFileNames(local.log, local.cubin, local.matog, tmpFolder, len);
			}
			
			// init code ptr
			blob code;
			
			// check if this target was not compiled in the meantime!
			if(!CUBINStore::doesExist(jit)) {
				// compile using JIT compiler
				code = Jit::compile(jit, local.log, local.cubin, local.matog);
			
				// do we have code?
				if(code.ptr == 0)
					THROWIF("CUBINSTORE_NO_CUBIN_AVAILABLE");
			}

			FREE_A(jit.getMatogCode());

			if(code.ptr) {
				// save to store
				CUBINStore::add(jit, code);

				// save to ModuleCache
				Context::get(jit.getContext()).getCache().add(jit, code);

				// free
				FREE_A(code.ptr);
			}

			{ // has to be locked
				ULOCK(m_mutex);

				// remove from working set
				m_currentlyCompiling.erase(jit);
			}
			
			// notify profiler
			Exec::notifyAll(jit);
		});
	}	

	return true;
}

//-------------------------------------------------------------------
			}
		}
	}
}