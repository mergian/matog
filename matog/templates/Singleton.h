// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_UTIL_SINGLETON_H
#define __MATOG_UTIL_SINGLETON_H

#include <matog/util/Mem.h>
#include <matog/util/Exit.h>
#include <matog/log.h>
#include <mutex>
#include <cassert>

namespace matog {
	namespace templates {
//-------------------------------------------------------------------
		template<class T>
		class Singleton {
		public:
			static T& getInstance(void) {
				if(!isInitialized()) {
					T::s_instance = NEW(T());
					matog::util::Exit::add(&free);
				}

				assert(T::s_instance);
				return *T::s_instance;
			}

			static void free(void) {
				if(T::s_instance) {
					FREE(T::s_instance);
					T::s_instance = 0;
				}
			}

			static bool isInitialized(void) {
				return T::s_instance != 0;
			}
		};

		template<class T>
		class ThreadedSingleton {
		public:
			static T& getInstance(void) {
				if(!isInitialized()) {
					T::s_instance = NEW(T());
					matog::util::Exit::add(&free);
				}

				assert(T::s_instance);
				return *T::s_instance;
			}

			static void free(void) {
				if(T::s_instance) {
					T::s_instance->join();

					FREE(T::s_instance);
					T::s_instance = 0;
				}
			}

			static bool isInitialized(void) {
				return T::s_instance != 0;
			}
		};

//-------------------------------------------------------------------
	}
}

#endif