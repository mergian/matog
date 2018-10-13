// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Converter.h"
#include <matog/runtime/intern/exec/Profiler.h>
#include <matog/runtime/intern/HostArray.h>
#include <matog/runtime/intern/Runtime.h>

#include <matog/Options.h>

#include <matog/util/Mem.h>

#include <matog/macros.h>
#include <matog/log.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
Converter::Converter(void) {
}

//-------------------------------------------------------------------
Converter::~Converter(void) {
	m_tasks.wait();
}

//-------------------------------------------------------------------
bool Converter::push(Profiler* profiler, HostMap* hostMap, std::mutex* mutex, const Hash arrayHash, const Hash kernelHash) {
	ULOCK(m_mutex);

	Item item(profiler, hostMap, mutex, arrayHash);
	m_notify.emplace(MAP_EMPLACE(item, kernelHash));

	// is this kernel already in the queue?
	auto cit = m_currentlyCompiling.find(item);

	// not in queue?
	if(cit == m_currentlyCompiling.end()) {
		m_currentlyCompiling.emplace(item);

		Item item(profiler, hostMap, mutex, arrayHash);
		
		m_tasks.run([item, this](){
			// get other. we expect the map to be filled with at least one element
			assert(!item.hostMap->empty());
			HostArray* other = item.hostMap->begin()->second;

			// convert from other
			HostArray* newArray = (HostArray*)NEW_P(other->clone(other->file(), other->line()));
			newArray->setConfig(item.hash);
			newArray->copy(other);

			{ // lock ArrayStore
				LOCK(*item.mutex);
				item.hostMap->emplace(MAP_EMPLACE(item.hash, newArray));
			}

			std::list<Hash> toNotify;
			
			{ // lock ConversionThreads
				ULOCK(m_mutex);

				// notify profiler
				auto range = m_notify.equal_range(item);
				for(; range.first != range.second; range.first++) {
					toNotify.emplace_back(range.first->second);
				}

				// remove from working set
				m_currentlyCompiling.erase(item);
				m_notify.erase(item);
			}
			
			for(auto& hash : toNotify)
				item.profiler->notifyConversion(hash);
		});
	}

	return true;
}

//-------------------------------------------------------------------
				}
			}
		}
	}
}