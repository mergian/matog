// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <matog/cuda/Architecture.h>
#include <matog/Degree.h>
#include <matog/degree/Type.h>
#include <matog/array/Type.h>
#include <matog/db/tables.h>
#include <matog/db/Stmt.h>
#include <matog/Static.h>
#include <matog/util/String.h>
#include <matog/macros.h>
#include <matog/degree/PrettyPrinter.h>

namespace matog {
//-------------------------------------------------------------------
using matog::db::Stmt;

//-------------------------------------------------------------------
void Degree::dbSelect(const uint32_t id) {
	invalidate();

	Stmt stmt("SELECT id, array_id, name, type, value_count, is_shared, is_compiler, is_function FROM " DB_DEGREES " WHERE id = ?;");
	stmt.bind(0, id);

	THROWIF(!stmt.step(), "degree not found");
	set(stmt);
}

//-------------------------------------------------------------------
void Degree::dbSelect(const char* name) {
	invalidate();

	Stmt stmt("SELECT id, array_id, name, type, value_count, is_shared, is_compiler, is_function FROM " DB_DEGREES " WHERE name = ?;");
	stmt.bind(0, name);

	THROWIF(!stmt.step(), "degree not found");
	set(stmt);
}

//-------------------------------------------------------------------
Degree::Degree(void) : 
	m_id			(0), 
	m_arrayId		(0), 
	m_valueCount	(0),
	m_name			(0),
	m_type			(degree::Type::None),
	m_array			(0),
	m_isShared		(false),
	m_isCompiler	(false),
	m_isFunction	(false),
	m_isRoot		(false)
{
	m_data.define = {0};
}

//-------------------------------------------------------------------
void Degree::set(const Stmt& stmt) {
	// free define strings and name if necessary
	free(true);

	// reset
	m_id			= stmt.get<uint32_t>(0);
	m_arrayId		= stmt.get<uint32_t>(1);
	m_valueCount	= stmt.get<uint32_t>(4);
	m_type			= stmt.get<degree::Type>(3);
	m_array			= 0;
	m_data.define	= {0};
	m_isShared		= stmt.get<bool>(5);
	m_isCompiler	= stmt.get<bool>(6);
	m_isFunction	= stmt.get<bool>(7);
	m_isRoot		= stmt.get<bool>(8);

	// copy name
	m_name = NEW_STRING(stmt.get<const char*>(2));

	// if is define, get strings and values
	if(getType() == degree::Type::Define) {
		// get value count
		{ 
			Stmt stmt("SELECT COUNT(*) FROM " DB_DEGREE_DEFINES " WHERE degree_id = ?;");
			stmt.bind(0, m_id);

			THROWIF(!stmt.step());
			m_valueCount = stmt.get<uint32_t>(0);
		}
		
		// alloc arrays
		assert(m_valueCount);
		m_data.define.strings	= NEW_A(const char*,	m_valueCount);
		m_data.define.values	= NEW_A(uint32_t,		m_valueCount);

		// get values
		{
			Stmt stmt("SELECT ordering, key, value FROM " DB_DEGREE_DEFINES " WHERE degree_id = ?;");
			stmt.bind(0, m_id);

			while(stmt.step()) {
				const uint32_t index			= stmt.get<uint32_t>(0);
				m_data.define.strings[index]	= NEW_STRING(stmt.get<const char*>(1));
				m_data.define.values [index]	= stmt.get<uint32_t>(2);
			}
		}
	} else if(getType() == degree::Type::L1Cache) {
		m_valueCount = 3; // SM, L1, EQ
	} else if(getType() == degree::Type::Range) {
		Stmt stmt("SELECT min, max, step FROM " DB_DEGREE_RANGES " WHERE degree_id = ?;");
		stmt.bind(0, m_id);

		THROWIF(!stmt.step(), "Range not found.");

		m_data.range.min  = stmt.get<double>(0);
		m_data.range.max  = stmt.get<double>(1);
		m_data.range.step = stmt.get<double>(2);

		m_valueCount = (uint32_t)((m_data.range.max - m_data.range.min) / m_data.range.step);
	}
}

//-------------------------------------------------------------------
Degree::~Degree(void) {
	free(true);
}

//-------------------------------------------------------------------
bool Degree::isGPULimited(void) const {
	return getType() == degree::Type::L1Cache;		
}

//-------------------------------------------------------------------
bool Degree::isFunction(void) const {
	return m_isFunction;
}

//-------------------------------------------------------------------
bool Degree::isShared(void) const {
	return m_isShared;
}

//-------------------------------------------------------------------
bool Degree::isIndependent(void) const {
	return !isShared();
}

//-------------------------------------------------------------------
bool Degree::isCompiler(void) const {
	return m_isCompiler;
}

//-------------------------------------------------------------------
cuda::Architecture Degree::getMinArch(const variant::Value value) const {
	if(m_type == degree::Type::L1Cache) {
		switch((L1Cache)value) {
		case L1Cache::SM:
			return cuda::Architecture::MIN_ARCH;
		case L1Cache::L1:
			return cuda::Architecture::FERMI;
		case L1Cache::EQ:
			return cuda::Architecture::KEPLER_30;
		default:
			;
		}
	}	
	
	return cuda::Architecture::MIN_ARCH;
}

//-------------------------------------------------------------------
cuda::Architecture Degree::getMaxArch(const variant::Value value) const {
	if(m_type == degree::Type::L1Cache) {
		switch((L1Cache)value) {
		case L1Cache::SM:
			return cuda::Architecture::MAX_ARCH;
		case L1Cache::L1:
		case L1Cache::EQ:
			return cuda::Architecture::KEPLER_35;
		default:
			;
		}
	}

	return cuda::Architecture::MAX_ARCH;
}

//-------------------------------------------------------------------
const char* Degree::printLong(const variant::Value value) const {
	switch(getType()) {
	case degree::Type::ArrayMemory:
		return degree::PrettyPrinter::printLong((Memory)value);
	case degree::Type::L1Cache:
		return degree::PrettyPrinter::printLong((L1Cache)value);
	case degree::Type::ArrayLayout:
		return degree::PrettyPrinter::printLong((Layout)value);
	case degree::Type::Define:
		return getDefineString(value);
	case degree::Type::Range:
	case degree::Type::ArrayTransposition:
	default:
		THROW("UNKNOWN_CONFIG_TYPE");
	};

	return 0;
}

//-------------------------------------------------------------------
char Degree::printShort(const variant::Value value) const {
	switch(getType()) {
	case degree::Type::ArrayMemory:
		return degree::PrettyPrinter::printShort((Memory)value);
	case degree::Type::L1Cache:
		return degree::PrettyPrinter::printShort((L1Cache)value);
	case degree::Type::ArrayLayout:
		return degree::PrettyPrinter::printShort((Layout)value);
	case degree::Type::Define:
		return m_data.define.strings[value][0];
	case degree::Type::Range:
	case degree::Type::ArrayTransposition:
	default:
		THROW("UNKNOWN_CONFIG_TYPE");
	};

	return '?';
}

//-------------------------------------------------------------------
void Degree::free(const bool all) {
	m_id = 0;

	if(all && m_name) {
		FREE_A(m_name);
		m_name = 0;
	}

	// for defines
	if(getType() == degree::Type::Define) {
		if(m_data.define.strings) {
			for(uint32_t i = 0; i < m_valueCount; i++)
				FREE_A(m_data.define.strings[i]);

			FREE_A(m_data.define.strings);
		}

		if(m_data.define.values)
			FREE_A(m_data.define.values);
		
		m_data.define = {0};
	}
	
	// for range
	else if(getType() == degree::Type::Range) {
		m_data.range = {0};	
	}
}

//-------------------------------------------------------------------
void Degree::invalidate(void) { 
	free(false);
}

//-------------------------------------------------------------------
bool Degree::isLocal(void) const {
	return !isGlobal();
}

//-------------------------------------------------------------------
bool Degree::isGlobal(void) const {
	if(!isArray())
		return false;

	if(getArray()->getType() != array::Type::Global)
		return false;

	return getType() == degree::Type::ArrayLayout || getType() == degree::Type::ArrayTransposition;
}

//-------------------------------------------------------------------
variant::Value Degree::getValueCount(const GPU& gpu) const {
	if(getType() == degree::Type::L1Cache) {
		const auto arch = gpu.getArchitecture();

		// FERMI
		if(arch >= cuda::Architecture::FERMI && arch <= cuda::Architecture::KEPLER_30)
			return 2;
		
		// KEPLER
		if(arch >= cuda::Architecture::KEPLER_30 && arch <= cuda::Architecture::KEPLER_35)
			return 3;

		// POST-KEPLER
		if(arch > cuda::Architecture::KEPLER_35)
			return 1;
	}

	return getValueCount();
}

//-------------------------------------------------------------------
bool Degree::isConfigType(const variant::Type type) const {
	switch(type) {
	case variant::Type::All:				return true;
	case variant::Type::Global:				return isGlobal();
	case variant::Type::Local:				return isLocal();
	case variant::Type::Module:				return isCompiler();
	case variant::Type::Shared:				return isShared();
	case variant::Type::Independent:		return isIndependent();
	case variant::Type::LocalIndependent:	return isIndependent() && isLocal();
	case variant::Type::Compiler:			return isCompiler();
	default: break;
	};

	THROW();
	return false;
}

//-------------------------------------------------------------------
uint32_t Degree::getDefineValue(const variant::Value value) const { 
	assert(getType() == degree::Type::Define); 
	return m_data.define.values[value]; 
}

//-------------------------------------------------------------------
const char*	Degree::getDefineString(const variant::Value value) const { 
	assert(getType() == degree::Type::Define); 
	return m_data.define.strings[value]; 
}

//-------------------------------------------------------------------
double Degree::getRangeValue(const variant::Value value) const { 
	assert(getType() == degree::Type::Range);
	return m_data.range.min + m_data.range.step * value; 
}

//-------------------------------------------------------------------
}