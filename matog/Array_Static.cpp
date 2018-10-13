// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <matog/macros.h>
#include <matog/degree/Type.h>
#include <matog/array/field/Type.h>

#include <matog/util/String.h>

#include <matog/Array.h>
#include <matog/array/Field.h>
#include <matog/array/Type.h>

#include <matog/db/tables.h>
#include <matog/db/Stmt.h>
#include <algorithm>

namespace matog {
//-------------------------------------------------------------------
using matog::util::String;
using matog::db::Stmt;
using matog::array::Field;

//-------------------------------------------------------------------
void Array::dbInsert(Field& root) {
	// prepare stmts
	Stmt stmt("INSERT INTO " DB_ARRAYS " (name, ifndef, type, is_struct, is_cube, is_32bitcount, vdim_count, root_dim_count, root_sizes, root_ptr_count) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");

	// insert global
	std::string ifndef("__");
	ifndef.append(root.getGlobalName());
	std::transform(ifndef.begin(), ifndef.end(), ifndef.begin(), ::toupper);

	L_DEBUG("inserting array: %s", root.getGlobalName());
	root.setGlobalId(stmt
		.bind(0, root.getGlobalName())
		.bind(1, ifndef.c_str())
		.bind(2, array::Type::Global)
		.bind(3, root.isStruct())
		.bind(4, root.isCube())
		.bind(5, root.getExternalIndexType() == array::field::Type::U32)
		.bind(6, root.getVDimCount())
		.bind(7, root.getDimCount())
		.bind(8, util::blob(&root.getCounts()[0], sizeof(uint32_t) * root.getDimCount()))
		.bind(9, root.getChildren().size() == 0 ? 1 : root.getChildren().size())
		.step32()
	);

	// init types
	std::set<array::field::Type> types;

	std::function<void (const Field& current)> helper = [&](const Field& current) {
		if(current.isStruct()) {
			for(const Field& c : current.getChildren())
				helper(c);
		} else {
			types.emplace(current.getType());
		}
	};

	helper(root);

	// insert shared
	ifndef = "__";
	ifndef.append(root.getSharedName());
	std::transform(ifndef.begin(), ifndef.end(), ifndef.begin(), ::toupper);

	L_DEBUG("inserting array: %s", root.getSharedName());
	stmt.reset();
	root.setSharedId(stmt
		.bind(0, root.getSharedName())
		.bind(1, ifndef.c_str())
		.bind(2, array::Type::Shared)
		.bind(3, false)
		.bind(6, true)
		.step32()
	);

	// insert dyn
	ifndef = "__";
	ifndef.append(root.getDynName());
	std::transform(ifndef.begin(), ifndef.end(), ifndef.begin(), ::toupper);

	L_DEBUG("inserting array: %s", root.getDynName());
	stmt.reset();
	root.setDynId(stmt
		.bind(0, root.getDynName())
		.bind(1, ifndef.c_str())
		.bind(2, array::Type::Dyn)
		.step32()
	);

	assert(root.getGlobalId());
	assert(root.getSharedId());
	assert(root.getDynId());

	// INSERT DEGREES
	stmt("INSERT INTO " DB_DEGREES " (array_id, name, type, value_count, is_compiler, is_function, is_shared, is_root) VALUES (?, UPPER(?), ?, ?, 1, 0, 0, ?);");
	std::list<const char*> names;

	std::function<void (Field&, const uint32_t)> func = [&](Field& field, const uint32_t mode) {
		stmt.reset();
		stmt.bind(4, field.isRoot());

		// init base name
		std::ostringstream ss;
		for(const char* name : names)
			ss << name << "_";

		const std::string base(ss.str());

		// transpositions
		if(field.isMultiDimensional()) {
			std::string name(base);	name.append("T");
			L_DEBUG("inserting degree: %s", name.c_str());
			stmt.reset();
			stmt.bind(1, name.c_str());
			stmt.bind(2, degree::Type::ArrayTransposition);
			stmt.bind(3, field.getTranspositionCount());
			
			if(mode == 0)		field.setGlobalTranspositionId	(stmt.step32());
			else if(mode == 1)	field.setSharedTranspositionId	(stmt.step32());
			else if(mode == 2)	field.setDynTranspositionId		(stmt.step32());
		}

		// layouts
		if(field.isLayoutStruct()) {
			std::string name(base);	name.append("L");
			L_DEBUG("inserting degree: %s", name.c_str());
			stmt.reset();
			stmt.bind(1, name.c_str());
			stmt.bind(2, degree::Type::ArrayLayout);
			stmt.bind(3, field.isRoot() ? 3 : 2); // AoSoA is only for the root element!
			
			if(mode == 0)		field.setGlobalLayoutId	(stmt.step32());
			else if(mode == 1)	field.setSharedLayoutId	(stmt.step32());
			else if(mode == 2)	field.setDynLayoutId	(stmt.step32());
		}
		
		// go through children
		if(field.isStruct()) {
			for(Field& sub : field.getChildren()) {
				names.emplace_back(sub.getName());
				func(sub, mode);
				names.pop_back();
			}
		}

		stmt.reset();
	};

	// global degrees
	names.emplace_back(root.getGlobalName());
	stmt.bind(0, root.getGlobalId());

	// memory
	func(root, 0);

	std::string name(root.getGlobalName()); name.append("_M");
	L_DEBUG("inserting degree: %s", name.c_str());
	stmt.bind(1, name.c_str());
	stmt.bind(2, degree::Type::ArrayMemory);
	stmt.bind(3, root.isConstant() ? 3 : 2);
	stmt.bind(4, true);
	root.setGlobalMemoryId(stmt.step32());
	stmt.reset();

	names.pop_back();

	// shared degrees
	names.emplace_back(root.getSharedName());
	stmt.bind(0, root.getSharedId());
	func(root, 1);
	names.pop_back();

	// dyn degrees
	names.emplace_back(root.getDynName());
	stmt.bind(0, root.getDynId());
	func(root, 2);
	names.pop_back();
}

//-------------------------------------------------------------------
bool Array::dbDoesExist(const uint32_t id) {
	Stmt stmt("SELECT COUNT(*) FROM " DB_ARRAYS " WHERE id = ?;");
	stmt.bind(0, id);
	THROWIF(!stmt.step());
	return stmt.get<uint32_t>(0) > 0;
}

//-------------------------------------------------------------------
void Array::dbSelect(Array& array, const uint32_t id) {
	array.invalidate();

	Stmt stmt("SELECT id, name, ifndef, type, is_struct, is_cube, is_32bitcount, vdim_count, root_dim_count, root_sizes, root_ptr_count FROM " DB_ARRAYS " WHERE id = ?;");
	stmt.bind(0, id);
	THROWIF(!stmt.step());
	array.set(stmt);
}

//-------------------------------------------------------------------
void Array::dbSelect(Array& array, const char* name) {
	array.invalidate();

	Stmt stmt("SELECT id, name, ifndef, type, is_struct, is_cube, is_32bitcount, vdim_count, root_dim_count, root_sizes, root_ptr_count FROM " DB_ARRAYS " WHERE name = ?;");
	stmt.bind(0, name);
	
	THROWIF(!stmt.step());
	array.set(stmt);
}

//-------------------------------------------------------------------
void Array::dbSelectAll(std::map<uint32_t, Array>& map) {
	Stmt stmt("SELECT id, name, ifndef, type, is_struct, is_cube, is_32bitcount, vdim_count, root_dim_count, root_sizes, root_ptr_count FROM " DB_ARRAYS ";");

	while(stmt.step())
		map.emplace(MAP_EMPLACE(stmt.get<uint32_t>(0))).first->second.set(stmt);
}

//-------------------------------------------------------------------
void Array::dbDelete(const uint32_t id) {
	Stmt("DELETE FROM " DB_ARRAYS " WHERE id = ?;").bind(0, id).step();
}

//-------------------------------------------------------------------
void Array::dbDelete(const char* name) {
	Stmt("DELETE FROM " DB_ARRAYS " WHERE name = ?;").bind(0, name).step();
}

//-------------------------------------------------------------------
void Array::dbDeleteAll(void) {
	Stmt("DELETE FROM " DB_ARRAYS";").step();
	Stmt("DELETE FROM sqlite_sequence WHERE name = '" DB_ARRAYS "';").step();
}

//-------------------------------------------------------------------
}