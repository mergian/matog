// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "AllocStore.h"
#include <matog/runtime/intern/exec/Params.h>
#include <matog/runtime/intern/Context.h>
#include <matog/log.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
AllocStore::AllocStore(const Params& params) : m_stream(params.stream) {
	void** args = params.args;

	for(uint32_t i = 0; args[i] != 0; i++) {
		CUdeviceptr ptr = *(CUdeviceptr*)args[i];
		uint64_t bytes = Context::get().getAllocationSize(ptr);

		// is this a registered Allocation?
		if(bytes > 0) {
			void* host = malloc(bytes);
			CHECK(::cuMemcpyDtoHAsync(host, ptr, bytes, m_stream));
			m_list.emplace_back(ptr, host, bytes);
		}
	}

	CHECK(::cuStreamSynchronize(m_stream));
}

//-------------------------------------------------------------------
AllocStore::~AllocStore(void) {
	for(auto& item : m_list) 
		free(item.host);
}

//-------------------------------------------------------------------
void AllocStore::restore(void) {
	for(auto& item : m_list) 
		CHECK(::cuMemcpyHtoDAsync(item.device, item.host, item.bytes, m_stream));

	CHECK(::cuStreamSynchronize(m_stream));
}

//-------------------------------------------------------------------
				}
			}
		}
	}
}