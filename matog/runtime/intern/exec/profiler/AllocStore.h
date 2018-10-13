// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_PROFILER_ALLOCSTORE
#define __MATOG_RUNTIME_INTERN_EXEC_PROFILER_ALLOCSTORE

#include <matog/runtime.h>
#include <list>
#include <cuda.h>
#include <cstdint>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
class AllocStore {
private:
	struct Item {
		CUdeviceptr device;
		void* host;
		uint64_t bytes;

		inline Item(CUdeviceptr _device, void* _host, uint64_t _bytes) : device(_device), host(_host), bytes(_bytes) {}
	};

	std::list<Item>	m_list;
	CUstream		m_stream;

public:
	AllocStore(const Params& params);
	~AllocStore(void);
	void restore();

	friend class exec::Profiler;
};

//-------------------------------------------------------------------
				}
			}
		}
	}
}

#endif