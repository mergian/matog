// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Cache.h"
#include <matog/log.h>
#include <matog/cuda/CUBINStore.h>
#include <matog/cuda/jit/Target.h>
#include <matog/cuda/jit/Item.h>
#include <matog/db/Stmt.h>
#include <cuda.h>
#include <sstream>
#include <matog/macros.h>

#define CACHE_SIZE 50

namespace matog {
	namespace runtime {
		namespace intern {
			namespace context {
//-------------------------------------------------------------------
Cache::Cache(CUcontext context) : m_context(context) {
}

//-------------------------------------------------------------------
Cache::~Cache(void) {
}

//-------------------------------------------------------------------
cache::Item& Cache::get(const jit::Target& target) {
	LOCK(m_mutex);

	auto it = m_items.find(target.getItem());

	// does item exist?
	if(it == m_items.end())
		return loadFromDB(target);

	return it->second;
}

//-------------------------------------------------------------------
void Cache::add(const jit::Target& target, const util::blob code) {
	assert(code.ptr);
	LOCK(m_mutex);
	CHECK(cuCtxSetCurrent(m_context));
	addCubin(target, code);
}

//-------------------------------------------------------------------
cache::Item& Cache::loadFromDB(const jit::Target& target) {
	db::Stmt stmt;
	cuda::CUBINStore::get(target, stmt);

	const util::blob code = stmt.get<util::blob>(0);

	THROWIF(code.ptr == 0, "cubin not found in database");
	return addCubin(target, code);
}

//-------------------------------------------------------------------
cache::Item& Cache::addCubin(const jit::Target& target, const util::blob code) {
	// This Method assumes that the MUTEX is locked!

	// Free Item if we have more than CACHE_SIZE loaded
	if(m_items.size() >= CACHE_SIZE) {
		Items::iterator candidate = m_items.begin();

		for(Items::iterator it = ++m_items.begin(); it != m_items.end(); it++) {
			if(candidate->second.getLastUsage() > it->second.getLastUsage())
				candidate = it;
		}

		// free
		m_items.erase(candidate);
	}

	return m_items.emplace(MAP_EMPLACE(target.getItem(), target.getItem(), code)).first->second;
}

//-------------------------------------------------------------------
			}
		}
	}
}