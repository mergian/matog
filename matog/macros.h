// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_MACROS
#define __MATOG_MACROS

// fix for gcc...
#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)(-1))
#endif

// lock
#define LOCK(MUTEX) std::lock_guard<std::mutex> _lock(MUTEX)
#define ULOCK(MUTEX) std::unique_lock<std::mutex> lock(MUTEX)

// maps
#define MAP_EMPLACE(key, ...) std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(__VA_ARGS__)
#endif