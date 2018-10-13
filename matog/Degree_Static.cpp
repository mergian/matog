// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <matog/Degree.h>
#include <matog/degree/Type.h>
#include <matog/array/Type.h>
#include <matog/db/tables.h>
#include <matog/db/Stmt.h>
#include <matog/Static.h>
#include <matog/util/String.h>
#include <matog/macros.h>
#include <algorithm>

namespace matog {
//-------------------------------------------------------------------
using matog::db::Stmt;

//-------------------------------------------------------------------
bool Degree::dbDoesExist(const uint32_t id) {
	Stmt stmt("SELECT COUNT(*) FROM " DB_DEGREES " WHERE id = ?;");
	stmt.bind(0, id);

	THROWIF(!stmt.step());
	return stmt.get<uint32_t>(0) > 0;
}

//-------------------------------------------------------------------
void Degree::dbSelectAll(std::map<uint32_t, Degree>& map) {
	Stmt stmt("SELECT id, array_id, name, type, value_count, is_shared, is_compiler, is_function, is_root FROM " DB_DEGREES ";");

	while(stmt.step())
		map.emplace(MAP_EMPLACE(stmt.get<uint32_t>(0))).first->second.set(stmt);	
}

//-------------------------------------------------------------------
void Degree::dbDelete(const uint32_t id) {
	Stmt("DELETE FROM " DB_DEGREES " WHERE id = ?;").bind(0, id).step();
	Stmt("DELETE FROM " DB_DEGREE_DEFINES " WHERE degree_id = ?;").bind(0, id).step();
	Stmt("DELETE FROM " DB_DEGREE_RANGES " WHERE degree_id = ?;").bind(0, id).step();
}

//-------------------------------------------------------------------
void Degree::dbDelete(const char* name) {
	Stmt("DELETE FROM " DB_DEGREE_DEFINES " WHERE degree_id = (SELECT id FROM " DB_DEGREES " WHERE name = ?);").bind(0, name).step();
	Stmt("DELETE FROM " DB_DEGREE_RANGES " WHERE degree_id = (SELECT id FROM " DB_DEGREES " WHERE name = ?);").bind(0, name).step();
	Stmt("DELETE FROM " DB_DEGREES " WHERE name = ?;").bind(0, name).step();
}

//-------------------------------------------------------------------
void Degree::dbDeleteAll(void) {
	Stmt("DELETE FROM " DB_DEGREES ";").step();
	Stmt("DELETE FROM " DB_DEGREE_DEFINES ";").step();
	Stmt("DELETE FROM " DB_DEGREE_RANGES ";").step();
	Stmt("DELETE FROM sqlite_sequence WHERE NAME = '" DB_DEGREES "';").step();
}


//-------------------------------------------------------------------
void Degree::dbInsertRange(const char* name, const double min, const double max, const double step, const bool isShared) {
	Stmt stmt("INSERT INTO " DB_DEGREES " (array_id, name, type, value_count, is_compiler, is_function, is_shared, is_root) VALUES (0, ?, ?, ?, 1, 0, ?, 0);");
	stmt.bind(0, name);
	stmt.bind(1, (int)degree::Type::Range);
	stmt.bind(2, std::ceil((max - min) / step));
	stmt.bind(3, isShared);
	const uint32_t id = stmt.step32();

	Stmt stmt2("INSERT INTO " DB_DEGREE_RANGES " (degree_id, min, max, step) VALUES (?, ?, ?, ?);");
	stmt2.bind(0, id);
	stmt2.bind(1, min);
	stmt2.bind(2, max);
	stmt2.bind(3, step);
	stmt.step();
}

//-------------------------------------------------------------------
void Degree::dbInsertDefine(const char* name, const std::list< std::pair<util::String, uint32_t> >& options, const bool isShared) {
	Stmt stmt("INSERT INTO " DB_DEGREES " (array_id, name, type, value_count, is_compiler, is_function, is_shared, is_root) VALUES (0, ?, ?, ?, 1, 0, ?, 0);");
	stmt.bind(0, name);
	stmt.bind(1, (int)degree::Type::Define);
	stmt.bind(2, options.size());
	stmt.bind(3, isShared);
	const uint32_t id = stmt.step32();

	Stmt stmt2("INSERT INTO " DB_DEGREE_DEFINES " (degree_id, ordering, key, value) VALUES (?, ?, ?, ?);");
	stmt2.bind(0, id);

	uint32_t index = 0;
	for(const auto& it : options) {
		stmt2.reset();
		stmt2.bind(1, index);
		stmt2.bind(2, it.first.c_str());
		stmt2.bind(3, it.second);
		stmt2.step();
		
		index++;
	}
}

//-------------------------------------------------------------------
}