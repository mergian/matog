// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_LOG_DLL_H
#define __MATOG_LOG_DLL_H

#ifdef WIN32
	#if defined(matog_log_EXPORTS) || defined(matog_log_debug_EXPORTS)
	#define MATOG_LOG_DLL __declspec(dllexport)
	#else
	#define MATOG_LOG_DLL __declspec(dllimport)
	#endif
#else
	#define MATOG_LOG_DLL
#endif

#endif