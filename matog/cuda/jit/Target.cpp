// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/cuda/jit/Target.h>

namespace matog {
	namespace cuda {
		namespace jit {
//-------------------------------------------------------------------
using variant::Hash;

//-------------------------------------------------------------------
Target::Target(void) :
	m_srcFile			(0),
	m_matogCode			(0),
	m_userFlags			(0),
	m_ctx				(0)
{}

//-------------------------------------------------------------------
Target::Target(const uint32_t moduleId, const uint32_t cc, const Hash moduleHash, const char* srcFile) :
	m_srcFile			(srcFile),
	m_matogCode			(0),
	m_userFlags			(0),
	m_ctx				(0),
	m_item				(moduleId, cc, moduleHash, 0, sizeof(size_t) * 8)
{}

//-------------------------------------------------------------------
Target::Target(const uint32_t moduleId, const uint32_t cc, const Hash moduleHash, const char* srcFile, const uint32_t userHash, const char* userFlags) :
	m_srcFile			(srcFile),
	m_matogCode			(0),
	m_userFlags			(userFlags),
	m_ctx				(0),
	m_item				(moduleId, cc, moduleHash, userHash, sizeof(size_t) * 8)
{}

//-------------------------------------------------------------------
Target::Target(const uint32_t moduleId, const uint32_t cc, const Hash moduleHash, const uint32_t userHash, const uint32_t arch) :
	m_srcFile			(0),
	m_matogCode			(0),
	m_userFlags			(0),
	m_ctx				(0),
	m_item				(moduleId, cc, moduleHash, userHash, arch)
{}

//-------------------------------------------------------------------
bool Target::operator<(const Target& other) const {
	return getItem() < other.getItem();
}

//-------------------------------------------------------------------
		}
	}
}