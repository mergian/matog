// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_CUDA_CUDACUBINSTORE_H
#define __MATOG_CUDA_CUDACUBINSTORE_H

#include <matog/util.h>
#include <matog/internal.h>
#include <matog/log/Result.h>
#include <matog/cuda/jit/Item.h>
#include <set>

namespace matog {
	namespace cuda {
#if WIN32
template class MATOG_INTERNAL_DLL std::set<jit::Item>;
#endif
	
//-------------------------------------------------------------------
/// Stores CUBIN images
class MATOG_INTERNAL_DLL CUBINStore {
	static	std::set<jit::Item> s_exists;		///< Stores existing CUBIN information

public:
	/// initializes the store
	static void init(void);

	/// gets the cubin for a specified JitTarget
	static void get(const jit::Target& jit, db::Stmt& stmt);

	/// adds a cubin for a specific JitTarget
	static void	add(const jit::Target& jit, const util::blob cubin);

	/// checks if a cubin exists
	static bool	doesExist(const jit::Target& jit);

	/// deletes all cubins from DB
	static void	dbDeleteAll(void);

	/// deletes all cubins for a specific module
	static	void dbDelete(const uint32_t moduleId);
};

//-------------------------------------------------------------------
	}
}
#endif