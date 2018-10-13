// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_PROFILER_ARRAYSTORE
#define __MATOG_RUNTIME_INTERN_EXEC_PROFILER_ARRAYSTORE

#include <map>
#include <matog/runtime.h>
#include <mutex>
#include <cuda.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
class ArrayStore {
private:
	struct ArrayMaps {
		HostMap					host;
		intern::DeviceArray*	device;
		inline ArrayMaps(void) : device(0) {}
	};

	std::mutex										m_mutex;
	std::map<uint32_t, ArrayMaps>					m_argMap;
	std::multimap<const Array*, ArrayMaps*>			m_arrayMap;
	CUstream										m_stream;
	const Kernel&									m_kernel;

public:
	ArrayStore(const DeviceArrays& deviceArrays, const Params& params, const Kernel& kernel);
	ArrayStore(const ArrayStore&) = delete;
	~ArrayStore(void);

	intern::HostArray*		getHostArray	(const uint32_t index, const variant::Hash arrayHash);
	intern::DeviceArray*	getDeviceArray	(const uint32_t index, const variant::Hash arrayHash);
	bool					convertArray	(uint32_t& notificationCount, Profiler* profiler, const Variant& variant, const variant::Hash kernelHash, const matog::array::Set& arrays);
};

//-------------------------------------------------------------------
				}
			}
		}
	}
}

#endif