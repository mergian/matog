// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/kernel/Meta.h>
#include <matog/db/tables.h>
#include <matog/db/Stmt.h>
#include <matog/log.h>

namespace matog {
	namespace kernel {
//-------------------------------------------------------------------
using matog::variant::Hash;
using matog::db::Stmt;

//-------------------------------------------------------------------
void Meta::operator()(const Stmt& stmt) {
	m_staticSharedMem	= stmt.get<uint32_t>(0);
	m_constMem			= stmt.get<uint32_t>(1);
	m_localMem			= stmt.get<uint32_t>(2);
	m_usedRegisters		= stmt.get<uint32_t>(3);
}

//-------------------------------------------------------------------
Meta::Meta(void) {
	invalidate();
}

//-------------------------------------------------------------------
void Meta::invalidate(void) {
	m_staticSharedMem	= 0;
	m_constMem			= 0;
	m_localMem			= 0;
	m_usedRegisters		= 0;
}

//-------------------------------------------------------------------
void Meta::dbSelect(Meta& meta, const variant::Hash moduleHash, const uint32_t userHash, const uint32_t cc, const uint32_t kernelId) {
	meta.invalidate();
	
	Stmt stmt("SELECT static_smem, const_mem, local_mem, used_registers FROM " DB_CUBINS_INFO " WHERE module_hash = ? AND user_hash = ? AND kernel_id = ? AND cc = ?;");
	stmt.bind(0, moduleHash);
	stmt.bind(1, userHash);
	stmt.bind(2, kernelId);
	stmt.bind(3, cc);

	if(stmt.step())
		meta(stmt);
}

//-------------------------------------------------------------------
void Meta::dbInsert(const Hash moduleHash, const uint32_t userHash, const uint32_t cc, const uint32_t kernelId, const uint32_t staticSharedMem, const uint32_t constMem, const uint32_t localMem, const uint32_t usedRegisters) {
	Stmt stmt("INSERT INTO " DB_CUBINS_INFO " (module_hash, user_hash, cc, kernel_id, static_smem, const_mem, local_mem, used_registers) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
	stmt.bind(0, moduleHash);
	stmt.bind(1, userHash);
	stmt.bind(2, cc);
	stmt.bind(3, kernelId);
	stmt.bind(4, staticSharedMem);
	stmt.bind(5, constMem);
	stmt.bind(6, localMem);
	stmt.bind(7, usedRegisters);
	stmt.step();
}

//-------------------------------------------------------------------
void Meta::dbDeleteByModule(const uint32_t moduleId) {
	Stmt("DELETE FROM " DB_CUBINS_INFO " WHERE kernel_id IN (SELECT kernel_id FROM " DB_KERNELS " WHERE module_id = ?);").bind(0, moduleId).step();
}

//-------------------------------------------------------------------
void Meta::dbDeleteByKernel(const uint32_t kernelId) {
	Stmt("DELETE FROM " DB_CUBINS_INFO " WHERE kernel_id = ?;").bind(0, kernelId).step();
}

//-------------------------------------------------------------------
void Meta::dbDeleteByCUBIN(const uint32_t cubinHash) {
	Stmt("DELETE FROM " DB_CUBINS_INFO " WHERE hash = ?;").bind(0, cubinHash).step();
}

//-------------------------------------------------------------------
void Meta::dbDeleteAll(void) {
	Stmt("DELETE FROM " DB_CUBINS_INFO ";").step();
}

//-------------------------------------------------------------------
	}
}