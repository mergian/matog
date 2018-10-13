// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_RUNTIME_INTERN_DECISIONS
#define __MATOG_RUNTIME_INTERN_DECISIONS

#include <matog/runtime.h>
#include "predictor/Meta.h"

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
class Decisions {
private:
	struct Ident {
		uint32_t hash;
		uint32_t arrayId;
		uint32_t kernelId;
		uint32_t gpuId;

		Ident(db::Stmt& stmt);
		Ident(const uint32_t _hash, const uint32_t _arrayId, const uint32_t _kernelId, const uint32_t _gpuId);

		bool operator<(const Ident& other) const;
	};

	static std::map<Ident, uint32_t> s_decisions;
	
	Decisions(void) = delete;
	
public:
	static uint32_t getId(const uint32_t hash, const char* file, const int line);
	static uint32_t getId(const MemArray<>* array, const Kernel* kernel, const GPU* gpu);
	static void init(void);
};

//-------------------------------------------------------------------
		}
	}
}
#endif