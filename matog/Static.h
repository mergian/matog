// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_STATIC
#define __MATOG_STATIC

#include <matog/dll.h>
#include <map>
#include <list>
#include "Array.h"
#include "GPU.h"
#include "Module.h"
#include <matog/util/SQLiteDB.h>
#include <matog/degree/Set.h>
#include <matog/array/Set.h>
#include <matog/Degree.h>
#include <matog/Kernel.h>

namespace matog {
//-------------------------------------------------------------------
#ifdef WIN32
template class MATOG_INTERNAL_DLL std::map<uint32_t, Array>;
template class MATOG_INTERNAL_DLL std::map<uint32_t, Module>;
template class MATOG_INTERNAL_DLL std::map<uint32_t, GPU>;
template class MATOG_INTERNAL_DLL std::map<CUdevice, GPU*>;
template class MATOG_INTERNAL_DLL std::map<const Module*, std::vector<array::Set> >;
#endif

//-------------------------------------------------------------------
/// Container class to store instances of Degree, Array, Module, Kernel and GPU
class MATOG_INTERNAL_DLL Static {
	static std::map<uint32_t, Degree>								s_degrees;				///< Degrees
	static std::map<uint32_t, Array>								s_arrays;				///< Arrays
	static std::map<uint32_t, Module>								s_modules;				///< Modules
	static std::map<uint32_t, Kernel>								s_kernels;				///< Kernels
	static std::map<uint32_t, GPU>									s_gpus;					///< GPUs
	static std::map<CUdevice, GPU*>									s_gpuByDevice;			///< GPU* by CUdevice
	static degree::Set												s_functionDegrees;		///< all function degrees
	static degree::Set												s_compilerDegrees;		///< all compiler degrees
	static degree::Set												s_sharedDegrees;		///< all shared degrees
	static degree::Set												s_independentDegrees;	///< all independent degrees
	static degree::Set												s_defines;				///< all degrees of type "define"

	static void initDegrees			(void);	///< inits Degrees
	static void initArrays			(void);	///< inits Arrays
	static void initModules			(void);	///< inits Modules
	static void initKernels			(void);	///< inits Kernels
	static void initGPUs			(void);	///< init GPUs

public:
	/// inits the Static container
	static void init				(void);

	static inline const std::map<uint32_t, Degree>&	getDegrees				(void) { return s_degrees; }						///< ...
	static inline const degree::Set&				getFunctionDegrees		(void) { return s_functionDegrees; }				///< ...
	static inline const degree::Set&				getCompilerDegrees		(void) { return s_compilerDegrees; }				///< ...
	static inline const degree::Set&				getSharedDegrees		(void) { return s_sharedDegrees; }					///< ...
	static inline const degree::Set&				getIndependentDegrees	(void) { return s_independentDegrees; }				///< ...
	static inline const degree::Set&				getDefines				(void) { return s_defines; }						///< ...
	static inline const std::map<uint32_t, Module>&	getModules				(void) { return s_modules; }						///< ...
	static inline const std::map<uint32_t, Kernel>&	getKernels				(void) { return s_kernels; }						///< ...
	static inline const std::map<uint32_t, GPU>&	getGPUs					(void) { return s_gpus; }							///< ...
	static inline const std::map<uint32_t, Array>&	getArrays				(void) { return s_arrays; }							///< ...			

	static const Degree&					getDegree				(const uint32_t id);								///< ...
	static Degree&							getEditableDegree		(const uint32_t id);								///< ...
	static const Array&						getArray				(const uint32_t id);								///< ...
	static const Array&						getArray				(const char* name);									///< ...
	static const Module&					getModule				(const uint32_t id);								///< ...
	static const Module&					getModule				(const char* name, const char* src);				///< ...
	static const Kernel&					getKernel				(const uint32_t id);								///< ...
	static Kernel&							getEditableKernel		(const uint32_t id);								///< ...
	static const Kernel&					getKernel				(const char* name, const uint32_t moduleId = 0);	///< ...
	static const GPU&						getGPU					(const uint32_t id);								///< ...
	static const GPU&						getGPUByDevice			(const CUdevice id);								///< ...
	static const GPU&						getCurrentGPU			(void);												///< ...

	static float							getVersion				(void);												///< gets the current MATOG version
};

//-------------------------------------------------------------------
}

#endif