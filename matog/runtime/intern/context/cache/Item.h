// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_CONTEXT_CACHE_ITEM
#define __MATOG_RUNTIME_INTERN_CONTEXT_CACHE_ITEM

#include <matog/util.h>
#include <matog/runtime.h>
#include <matog/kernel/Meta.h>
#include <matog/cuda/jit/Item.h>
#include <map>
#include <cuda.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace context {
				namespace cache {
//-------------------------------------------------------------------
using matog::Kernel;

//-------------------------------------------------------------------
class Item {
	struct St {
		CUfunction func;
		kernel::Meta meta;

		inline St(CUfunction _func) : func(_func) {}
	};

	const cuda::jit::Item		m_jitItem;
	CUmodule					m_module;
	std::map<const Kernel*, St>	m_functions;
	uint64_t					m_timestamp;

	const St& get(const Kernel& kernel);

public:
	Item	(const cuda::jit::Item& item, const util::blob code);
	Item	(const Item&) = delete; // prevent copying!
	~Item	(void);

	inline	const cuda::jit::Item&	getItem			(void) const			{ return m_jitItem; }
	inline	const kernel::Meta&		getMeta			(const Kernel& kernel)	{ return get(kernel).meta; }
	inline	CUfunction				getFunction		(const Kernel& kernel)	{ return get(kernel).func; }
	inline	CUmodule				getModule		(void) const { return m_module; }
	inline	uint64_t				getLastUsage	(void) const { return m_timestamp; }	
};

//------------------------------------------------------------------
				}
			}
		}
	}
}

#endif