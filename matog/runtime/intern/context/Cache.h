// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_CONTEXT_CACHE
#define __MATOG_RUNTIME_INTERN_CONTEXT_CACHE

#include <matog/util.h>
#include <matog/runtime.h>
#include <matog/cuda/jit/Item.h>
#include "cache/Item.h"
#include <map>
#include <queue>
#include <mutex>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace context {
//-------------------------------------------------------------------
using namespace matog::cuda;

//-------------------------------------------------------------------
class Cache {
private:
	typedef std::map<jit::Item, cache::Item> Items;
	std::mutex		m_mutex;
	Items			m_items;
	CUcontext		m_context;

	cache::Item&	loadFromDB		(const jit::Target& target);
	cache::Item&	addCubin		(const jit::Target& target, const util::blob code);

public:
					Cache			(CUcontext context);
					Cache			(const Cache&) = delete; // necessary because of the mutex
	virtual			~Cache			(void);
	cache::Item&	get				(const jit::Target& target);
	void			add				(const jit::Target& target, const util::blob code);
};

//-------------------------------------------------------------------
			}
		}
	}
}
#endif