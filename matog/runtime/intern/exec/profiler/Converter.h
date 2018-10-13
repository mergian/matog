// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC_PROFILER_CONVERTER
#define __MATOG_RUNTIME_INTERN_EXEC_PROFILER_CONVERTER

#include <matog/runtime.h>
#include <set>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <tbb/task_group.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
class Converter {
	// item
	struct Item {
		Profiler*				profiler;
		HostMap*				hostMap;
		std::mutex*				mutex;
		matog::variant::Hash	hash;

		inline Item(void) :	profiler(0), hostMap(0) {}

		inline Item(Profiler* _profiler, HostMap* _hostMap, std::mutex* _mutex, const variant::Hash _hash) :
			profiler(_profiler),
			hostMap(_hostMap),
			mutex(_mutex),
			hash(_hash)
		{}

		inline bool operator<(const Item& other) const {
			if(profiler < other.profiler) {
				return true;
			} else if(profiler == other.profiler) {
				if(hostMap < other.hostMap) {
					return true;
				} else if(hostMap == other.hostMap) {
					return hash < other.hash;
				}
			}

			return false;
		}
	};

	// instance
	std::multimap<Item, variant::Hash>	m_notify;
	std::set<Item>						m_currentlyCompiling;
	std::mutex							m_mutex;
	tbb::task_group						m_tasks;

public:
	Converter	(void);
	~Converter	(void);
	bool push	(Profiler* profiler, HostMap* hostMap, std::mutex* mutex, const variant::Hash arrayHash, const variant::Hash kernelHash);
};

//-------------------------------------------------------------------
				}
			}
		}
	}
}

#endif