// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CUDA_JIT_ITEM
#define __MATOG_CUDA_JIT_ITEM

#include <matog/dll.h>
#include <matog/internal.h>

namespace matog {
	namespace cuda {
		namespace jit {
//-------------------------------------------------------------------
using matog::variant::Hash;

//-------------------------------------------------------------------
/// Information Item for the JitTarget
class MATOG_INTERNAL_DLL Item {
	uint32_t	m_moduleId;				///< ID of module to be compiled
	uint32_t	m_cc;					///< Target Compute Capability
	Hash		m_moduleHash;			///< Hash representing the configuration
	uint32_t	m_userHash;				///< Hash representing user defined compiler flags
	uint32_t	m_arch;					///< 32 or 64 bit application

public:
	/// default constuctor
	inline Item(void) : m_moduleId(0), m_cc(0), m_moduleHash(0), m_userHash(0), m_arch(0) {}

	/// inits the Item
	inline Item(const uint32_t moduleId, const uint32_t cc, const Hash moduleHash, const uint32_t userHash, const uint32_t arch) : 
		m_moduleId(moduleId), m_cc(cc), m_moduleHash(moduleHash), m_userHash(userHash), m_arch(arch) {}

	inline	uint32_t	getCC			(void) const { return m_cc; }				///< gets compute capability
	inline	uint32_t	getArch			(void) const { return m_arch; }				///< gets architecture (32/64 bit)
	inline	Hash		getModuleHash	(void) const { return m_moduleHash; }		///< hash representing the configurations
	inline	uint32_t	getUserHash		(void) const { return m_userHash; }			///< hash representing user compiler flags
	inline	uint32_t	getModuleId		(void) const { return m_moduleId; }			///< module id
			bool		operator<		(const Item& other) const;					///< compares two Items

	friend class Target;
};

//-------------------------------------------------------------------
		}
	}
}
#endif