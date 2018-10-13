// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME
#define __MATOG_RUNTIME

#include <matog/internal.h>
#include <matog/runtime/dll.h>
#include <cstdint>
#include <map>

namespace matog {
	enum class L1Cache;
	enum class Layout;
	enum class Memory;

	namespace runtime {
		class MATOG_RUNTIME_DLL BaseArray;
		class MATOG_RUNTIME_DLL MemArray;
		class MATOG_RUNTIME_DLL HostArray;
		class MATOG_RUNTIME_DLL DeviceArray;
		class MATOG_RUNTIME_DLL DynArray;
		
		namespace intern {
			class Controller;
			class Exec;
			class Decisions;
			class Predictor;
			class Context;
			class DeviceArray;
			class DynArray;
			class HostArray;
			template<class T = runtime::BaseArray> class BaseArray;
			template<class T = runtime::MemArray> class MemArray;
		
			namespace context {
				class Cache;
				class Meta;

				namespace cache {
					class Item;
				}

				namespace meta {
					enum class Type;
				}
			}
		
			namespace exec {
				typedef std::map<uint32_t, intern::DeviceArray*>	DeviceArrays;
				typedef std::map<uint32_t, intern::DynArray*>		DynSharedArrays;

				class Optimized;
				class Baseline;
				class Profiler;
				class Compiler;
				struct Params;
				enum class Mode;

				namespace profiler {
					typedef std::map<matog::variant::Hash, intern::HostArray*> HostMap;

					class Brute;
					class Plain;
					class Prediction;
					class AllocStore;
					class ArrayStore;
					class CUPTI;
				}
			}

			namespace predictor {
				struct Data;
				class Meta;
				class Item;
			}
		}
	}
}
		
#endif