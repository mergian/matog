// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_UTIL_DLL_H
#define __MATOG_UTIL_DLL_H

#ifdef WIN32
	#if defined(matog_util_EXPORTS) || defined(matog_util_debug_EXPORTS)
	#define MATOG_UTIL_DLL __declspec(dllexport)
	#else
	#define MATOG_UTIL_DLL __declspec(dllimport)
	#endif
#else
	#define MATOG_UTIL_DLL
#endif

#endif