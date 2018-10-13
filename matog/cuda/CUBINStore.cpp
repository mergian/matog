// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/cuda/CUBINStore.h>
#include <matog/cuda/jit/Item.h>
#include <matog/cuda/jit/Target.h>
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/log.h>
#include <mutex>
#include <cstdint>
#include <matog/macros.h>

namespace matog {
	namespace cuda {
//-------------------------------------------------------------------
using matog::db::Stmt;
using matog::util::blob;

std::set<jit::Item> CUBINStore::s_exists;
static std::mutex s_mutex;

//-------------------------------------------------------------------
void CUBINStore::init(void) {
	LOCK(s_mutex);

	Stmt stmt("SELECT module_id, cc, module_hash, user_hash, arch FROM " DB_CUBINS ";");
	
	while(stmt.step())
		s_exists.emplace(
			stmt.get<uint32_t>(0),
			stmt.get<uint32_t>(1),
			stmt.get<uint64_t>(2),
			stmt.get<uint32_t>(3),
			stmt.get<uint32_t>(4)
		);
}

//-------------------------------------------------------------------
void CUBINStore::get(const jit::Target& jit, db::Stmt& stmt) {
	LOCK(s_mutex);

	stmt("SELECT code FROM " DB_CUBINS " WHERE module_id = ? AND module_hash = ? AND user_hash = ? AND arch = ? AND cc = ?;");
	stmt.bind(0, jit.getModuleId());
	stmt.bind(1, jit.getModuleHash());
	stmt.bind(2, jit.getUserHash());
	stmt.bind(3, jit.getArch());
	stmt.bind(4, jit.getCC());
	stmt.step();
}

//-------------------------------------------------------------------
void CUBINStore::add(const jit::Target& jit, const blob cubin) {
	LOCK(s_mutex);

	Stmt("INSERT INTO " DB_CUBINS " (module_id, module_hash, user_hash, arch, cc, code) VALUES (?, ?, ?, ?, ?, ?);")
		.bind(0, jit.getModuleId())
		.bind(1, jit.getModuleHash())
		.bind(2, jit.getUserHash())
		.bind(3, jit.getArch())
		.bind(4, jit.getCC())
		.bind(5, cubin)
		.step();

	Stmt("INSERT OR IGNORE INTO " DB_CUBINS_FLAGS " (user_hash, user_flags) VALUES (?, ?);")
		.bind(0, jit.getUserHash())
		.bind(1, jit.getUserFlags())
		.step();

	s_exists.emplace(jit.getItem());
}

//-------------------------------------------------------------------
bool CUBINStore::doesExist(const jit::Target& jit) {
	LOCK(s_mutex);
	return s_exists.find(jit.getItem()) != s_exists.end();
}

//-------------------------------------------------------------------
void CUBINStore::dbDeleteAll(void) {
	Stmt("DELETE FROM " DB_CUBINS ";").step();
	Stmt("DELETE FROM " DB_CUBINS_INFO ";").step();

	// clear s_exists
	s_exists.clear();
}

//-------------------------------------------------------------------
void CUBINStore::dbDelete(const uint32_t moduleId) {
	Stmt("DELETE FROM " DB_CUBINS_INFO " WHERE kernel_id IN (SELECT kernel_id FROM " DB_KERNELS " WHERE module_id = ?);")
		.bind(0, moduleId).step();

	Stmt("DELETE FROM " DB_CUBINS " WHERE module_id = ?;")
		.bind(0, moduleId).step();

	// remove all instances of the module in the s_exists!
	auto it = s_exists.begin();

	while(it != s_exists.end()) {
		if(it->getModuleId() == moduleId)
			it = s_exists.erase(it);
		else
			it++;
	}
}

//-------------------------------------------------------------------
	}
}