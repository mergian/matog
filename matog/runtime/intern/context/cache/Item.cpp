// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Item.h"
#include <matog/util/Mem.h>
#include <matog/Kernel.h>
#include <matog/log.h>
#include <matog/macros.h>

using matog::util::blob;

namespace matog {
	namespace runtime {
		namespace intern {
			namespace context {
				namespace cache {
//-------------------------------------------------------------------
Item::Item(const cuda::jit::Item& item, const blob code) : m_jitItem(item), m_module(0), m_timestamp(log::getMicroSeconds()) {
	// load module
	CHECK(cuModuleLoadData(&m_module, code.ptr));
}

//-------------------------------------------------------------------
Item::~Item(void) {
	if(m_module) {
		CUresult result = cuModuleUnload(m_module);

		if(result != CUDA_SUCCESS || result != CUDA_ERROR_NOT_INITIALIZED)
			CHECK(result);

		m_module	= 0;
	}
}

//-------------------------------------------------------------------
const Item::St& Item::get(const Kernel& kernel) {
	std::map<const Kernel*, St>::iterator it = m_functions.find(&kernel);

	if(it == m_functions.end()) {
		CUfunction func;
		CHECK(cuModuleGetFunction(&func, getModule(), kernel.getName()));
		it = m_functions.emplace(MAP_EMPLACE(&kernel, func)).first;

		auto& meta = it->second.meta;
		kernel::Meta::dbSelect(meta, m_jitItem.getModuleHash(), m_jitItem.getUserHash(), m_jitItem.getCC(), kernel.getId());

		if(!meta.doesExist()) {
			int staticSharedMem = 0;
			int constMem = 0;
			int localMem = 0;
			int usedRegisters = 0;

			CHECK(cuFuncGetAttribute(&staticSharedMem,	CU_FUNC_ATTRIBUTE_SHARED_SIZE_BYTES,	func));
			CHECK(cuFuncGetAttribute(&constMem,			CU_FUNC_ATTRIBUTE_CONST_SIZE_BYTES,		func));
			CHECK(cuFuncGetAttribute(&localMem,			CU_FUNC_ATTRIBUTE_LOCAL_SIZE_BYTES,		func));
			CHECK(cuFuncGetAttribute(&usedRegisters,	CU_FUNC_ATTRIBUTE_NUM_REGS,				func));

			kernel::Meta::dbInsert(m_jitItem.getModuleHash(), m_jitItem.getUserHash(), m_jitItem.getCC(), kernel.getId(), staticSharedMem, constMem, localMem, usedRegisters);
			kernel::Meta::dbSelect(meta, m_jitItem.getModuleHash(), m_jitItem.getUserHash(), m_jitItem.getCC(), kernel.getId());
			assert(meta.doesExist());
		}
	}

	m_timestamp = log::getMicroSeconds();
	return it->second;
}

//-------------------------------------------------------------------
				}
			}
		}
	}
}