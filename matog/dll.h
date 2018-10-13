// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DLL
#define __MATOG_DLL

#ifdef WIN32
	#if defined(matog_internal_EXPORTS) || defined(matog_internal_debug_EXPORTS)
	#define MATOG_INTERNAL_DLL __declspec(dllexport)
	#else
	#define MATOG_INTERNAL_DLL __declspec(dllimport)
	#endif
#else
	#define MATOG_INTERNAL_DLL
#endif

#endif