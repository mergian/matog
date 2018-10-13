// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/Kernel.h>
#include <matog/db/tables.h>
#include <matog/db/Stmt.h>
#include <matog/log.h>
#include <matog/macros.h>
#include <matog/util/Hash.h>
#include <matog/Static.h>

using matog::db::Stmt;

namespace matog {
//-------------------------------------------------------------------
void Kernel::dbSelectAll(std::map<uint32_t, Kernel>& map) {
	Stmt stmt("SELECT id, name, module_id FROM " DB_KERNELS ";");

	while(stmt.step()) 
		map.emplace(MAP_EMPLACE(stmt.get<uint32_t>(0))).first->second.set(stmt);
}

//-------------------------------------------------------------------
void Kernel::dbSelect(const uint32_t id) {
	invalidate();

	Stmt stmt("SELECT id, name, module_id FROM " DB_KERNELS " WHERE id = ?;");
	stmt.bind(0, id);

	THROWIF(!stmt.step());
	set(stmt);
}

//-------------------------------------------------------------------
void Kernel::dbSelect(const char* name) {
	invalidate();

	Stmt stmt("SELECT id, name, module_id FROM " DB_KERNELS " WHERE name = ?;");
	stmt.bind(0, name);

	THROWIF(!stmt.step());
	set(stmt);
}

//-------------------------------------------------------------------
uint32_t Kernel::dbSelectId(const char* name) {
	Stmt stmt("SELECT id FROM " DB_KERNELS " WHERE name = ?;");
	stmt.bind(0, name);

	if(stmt.step())
		return stmt.get<uint32_t>(0);

	return UINT_MAX;
}

//-------------------------------------------------------------------
void Kernel::dbSelectOrInsert(const char* name, const uint32_t moduleId) {
	dbSelect(name);
	
	if(!doesExist()) {
		dbSelect(dbInsert(name, moduleId));
	}

	assert(doesExist());
}

//-------------------------------------------------------------------
uint32_t Kernel::dbInsert(const char* name, const uint32_t moduleId) {
	return Stmt("INSERT INTO " DB_KERNELS " (name, module_id) VALUES (?, ?);")
		.bind(0, name)
		.bind(1, moduleId)
		.step32();
}

//-------------------------------------------------------------------
void Kernel::dbDelete(const uint32_t moduleId) {
	Stmt("DELETE FROM " DB_KERNEL_DEGREES " WHERE kernel_id = (SELECT id FROM " DB_KERNELS " WHERE module_id = ?);").bind(0, moduleId).step();
	Stmt("DELETE FROM " DB_KERNELS " WHERE module_id = ?;").bind(0, moduleId).step();
}

//-------------------------------------------------------------------
void Kernel::dbDeleteAll(void) {
	Stmt("DELETE FROM " DB_KERNELS ";").step();
	Stmt("DELETE FROM sqlite_sequence WHERE NAME = '" DB_KERNELS "';").step();
	Stmt("DELETE FROM " DB_KERNEL_DEGREES ";").step();
	Stmt("DELETE FROM " DB_KERNEL_HINTS_REF_INDEX ";").step();
}

//-------------------------------------------------------------------
}