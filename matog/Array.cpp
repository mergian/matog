// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <matog/macros.h>
#include <matog/degree/Type.h>

#include <matog/util/String.h>
#include <matog/Static.h>

#include <matog/Array.h>
#include <matog/array/Field.h>
#include <matog/array/field/Type.h>
#include <matog/array/Type.h>

#include <matog/db/tables.h>
#include <matog/db/Stmt.h>

namespace matog {
//-------------------------------------------------------------------
using matog::util::String;
using matog::db::Stmt;

//-------------------------------------------------------------------
Array::Array(void) :
	m_id					(0),
	m_name					(0),
	m_ifndef				(0),
	m_rootSizes				(0),
	m_vdimCount				(0),
	m_rootDimCount			(0),
	m_rootPtrCount			(0),
	m_type					(array::Type::None),
	m_isStruct				(false),
	m_isCube				(false),
	m_is32BitCount			(false),
	m_rootLayout			(0),
	m_rootMemory			(0),
	m_rootTransposition		(0)
{}

//-------------------------------------------------------------------
void Array::set(const Stmt& stmt) {
	free();
	
	m_id					= stmt.get<uint32_t>(0);
	m_type					= stmt.get<array::Type>(3);
	m_isStruct				= stmt.get<bool>(4);
	m_isCube				= stmt.get<bool>(5);
	m_is32BitCount			= stmt.get<bool>(6);
	m_vdimCount				= stmt.get<uint32_t>(7);
	m_rootDimCount			= stmt.get<uint32_t>(8);
	m_rootPtrCount			= stmt.get<uint32_t>(10);

	m_name		= NEW_STRING(stmt.get<const char*>(1));
	m_ifndef	= NEW_STRING(stmt.get<const char*>(2));
	m_rootSizes = NEW_A(uint32_t, getRootDimCount());
	
	memcpy(m_rootSizes, stmt.get<util::blob>(9).ptr, sizeof(uint32_t) * getRootDimCount());
}

//-------------------------------------------------------------------
void Array::free(void) {
	if(m_name)
		FREE_A(m_name);

	if(m_ifndef)
		FREE_A(m_ifndef);

	if(m_rootSizes)
		FREE_A(m_rootSizes);
}

//-------------------------------------------------------------------
Array::~Array(void) {
	free();
}

//-------------------------------------------------------------------
uint32_t Array::getValueCount(const variant::Type type) const {
	uint32_t cnt = 1;

	for(const auto& it : getItems()) {
		if(it.getDegree()->isConfigType(type))
			cnt *= it.getDegree()->getValueCount();
	}

	return cnt;
}

//-------------------------------------------------------------------
void Array::dbSelectDegrees(std::map<uint32_t, Array>& map) {
	Stmt stmt("SELECT id FROM " DB_DEGREES " WHERE array_id = ?;");

	for(auto& it : map) {
		Array& array = it.second;
		stmt.reset();
		stmt.bind(0, array.getId());

		while(stmt.step()) {
			Degree* d = &Static::getEditableDegree(stmt.get<uint32_t>(0));
			d->m_array = &array;
			array.m_items.emplace(d, 0);

			if(d->isRoot()) {
				if(d->getType() == degree::Type::ArrayLayout)				array.m_rootLayout = d;
				else if(d->getType() == degree::Type::ArrayMemory)			array.m_rootMemory = d;
				else if(d->getType() == degree::Type::ArrayTransposition)	array.m_rootTransposition = d;
			}
		}

		array.m_variant.init(array.m_items);
	}
}

//-------------------------------------------------------------------
}