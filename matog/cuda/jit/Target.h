// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CUDA_JIT_TARGET
#define __MATOG_CUDA_JIT_TARGET

#include <cuda.h>
#include <matog/cuda/jit/Item.h>

namespace matog {
	namespace cuda {
		namespace jit {
//-------------------------------------------------------------------
using variant::Hash;

//-------------------------------------------------------------------
/// Target information for JIT compilation
class MATOG_INTERNAL_DLL Target {
	const	char*		m_srcFile;			///< kernel source code
	const	char*		m_matogCode;		///< CPP code which defines the configuration parameters for the device code
	const	char*		m_userFlags;		///< user defined compiler flags
			CUcontext	m_ctx;				///< GPU context
			Item		m_item;				///< Target informations

public:
	/// default constructor
	Target(void);

	/// builds target
	Target(const uint32_t moduleId, const uint32_t cc, const Hash moduleHash, const char* srcFile);

	/// builds target 
	Target(const uint32_t moduleId, const uint32_t cc, const Hash moduleHash, const char* srcFile, const uint32_t userHash, const char* userFlags);

	/// builds target
	Target(const uint32_t moduleId, const uint32_t cc, const Hash moduleHash, const uint32_t userHash, const uint32_t arch);

	/// sets the architecture
	inline	void			setArch			(const uint32_t arch) { m_item.m_arch = arch; }

	/// sets the MATOG Code
	inline	void			setMatogCode	(const char* matogCode) { m_matogCode = matogCode; }

	/// sets the context
	inline	void			setContext		(CUcontext ctx) { m_ctx = ctx; }

	inline	CUcontext		getContext		(void) const { return m_ctx; }						///< gets context
	inline	const Item&		getItem			(void) const { return m_item; }						///< gets target information
	inline	uint32_t		getCC			(void) const { return m_item.getCC(); }				///< gets CC
	inline	uint32_t		getArch			(void) const { return m_item.getArch(); }			///< gets architecture
	inline	const char*		getSourceFile	(void) const { return m_srcFile; }					///< gets source file
	inline	const char*		getMatogCode	(void) const { return m_matogCode; }				///< gets matog configuration code
	inline	const char*		getUserFlags	(void) const { return m_userFlags; }				///< gets user defined compiler flags
	inline	Hash			getModuleHash	(void) const { return m_item.getModuleHash(); }		///< gets module configuration hash
	inline	uint32_t		getUserHash		(void) const { return m_item.getUserHash(); }		///< gets user compiler flags hash
	inline	uint32_t		getModuleId		(void) const { return m_item.getModuleId(); }		///< gets module id
			bool			operator<		(const Target& other) const;						///< compares two JitTargets
};		

//-------------------------------------------------------------------
		}
	}
}

#endif