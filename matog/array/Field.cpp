// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Field.h"
#include "field/Type.h"
#include <matog/log.h>
#include <matog/util/Mem.h>
#include <matog/Static.h>
#include <matog/Degree.h>
#include <algorithm>

namespace matog {
	namespace array {
//-------------------------------------------------------------------
Field::Field(const Field* parent) :
	m_name					(0), 
	m_globalName			(0),
	m_sharedName			(0), 
	m_dynName				(0),
	m_type					(field::Type::UNKNOWN),
	m_parent				(parent),
	m_globalId				(0),
	m_globalLayoutId		(0),
	m_globalTranspositionId	(0),
	m_globalMemoryId		(0),
	m_sharedId				(0),
	m_sharedLayoutId		(0),
	m_sharedTranspositionId	(0),    
	m_dynId					(0),
	m_dynLayoutId			(0),
	m_dynTranspositionId	(0),
	m_internalIndexType		(field::Type::U32),
	m_externalIndexType		(field::Type::U32),
	m_isCube				(false),
	m_customCount			(0)
{}

//-------------------------------------------------------------------
void Field::operator()(const char* name, const field::Type type) {
	assert(m_type == field::Type::UNKNOWN);
	assert(m_name == 0);

	m_name = NEW_STRING(name);
	m_type = type;
}

//-------------------------------------------------------------------
Field& Field::addChild() {
	assert(getType() == field::Type::STRUCT);
	m_sub.emplace_back(this);
	return m_sub.back();
}

//-------------------------------------------------------------------
void Field::addCount(const uint32_t count) {
	m_counts.emplace_back(count);
}

//-------------------------------------------------------------------
bool Field::hasChildren(void) const {
	return !m_sub.empty();
}

//-------------------------------------------------------------------
const char*	Field::getName(void) const {
	return m_name;
}

//-------------------------------------------------------------------
void Field::setCube(const bool value) {
	m_isCube = value;
}

//-------------------------------------------------------------------
void Field::initNames(void) {
	assert(isStruct() || isDimensional());
	assert(m_globalName == 0);
	assert(m_sharedName == 0);
	assert(m_dynName == 0);

	{
		std::ostringstream ss;
		ss << getName() << "Global";
		m_globalName = NEW_STRING(ss.str().c_str());
	}

	{
		std::ostringstream ss;
		ss << getName() << "Shared";
		m_sharedName = NEW_STRING(ss.str().c_str());
	}

	{
		std::ostringstream ss;
		ss << getName() << "Dyn";
		m_dynName = NEW_STRING(ss.str().c_str());
	}
}

//-------------------------------------------------------------------
const char*	Field::getSharedName(void) const {
	assert(isStruct() || isDimensional());
	assert(m_sharedName);
	return m_sharedName;
}

//-------------------------------------------------------------------
const char*	Field::getGlobalName(void) const {
	assert(isStruct() || isDimensional());
	assert(m_globalName);
	return m_globalName;
}

//-------------------------------------------------------------------
const char*	Field::getDynName(void) const {
	assert(isStruct() || isDimensional());
	assert(m_dynName);
	return m_dynName;
}

//-------------------------------------------------------------------
Field::~Field(void) {
	if(m_name)			FREE_A(m_name);
	if(m_globalName)	FREE_A(m_globalName);
	if(m_sharedName)	FREE_A(m_sharedName);
	if(m_dynName)		FREE_A(m_dynName);
	if(m_customCount)	FREE_A(m_customCount);

	if(!m_indexes.empty())
		for(const char* ptr : m_indexes)
			FREE_A(ptr);
}

//-------------------------------------------------------------------
uint32_t Field::getSize(void) const {
	if(m_type == field::Type::STRUCT) {
		uint32_t size = 0;
		for(const Field& f : m_sub)
			size = std::max(size, f.getSize());
		return size;
	}

	return field::getTypeSize(m_type);
}

//-------------------------------------------------------------------
uint32_t Field::getTreeSize(void) const {
	if(m_type == field::Type::STRUCT) {
		uint32_t size = 0;
		for(const Field& f : m_sub)
			size += f.getSize();
		return size;
	}

	return field::getTypeSize(m_type);
}

//-------------------------------------------------------------------
bool Field::isFloat(void) const {
	return m_type == field::Type::FLOAT || m_type == field::Type::DOUBLE;
}

//-------------------------------------------------------------------
bool Field::isUnsigned(void) const {
	return m_type >= field::Type::U8 && m_type <= field::Type::U64;
}

//-------------------------------------------------------------------
const Field* Field::getParent(void) const {
	return m_parent;
}

//-------------------------------------------------------------------
bool Field::isRoot(void) const {
	return getParent() == 0;
}

//-------------------------------------------------------------------
bool Field::operator<(const Field& field) const {
	if(isStruct() && !field.isStruct())	return true;
	if(!isStruct() && field.isStruct()) return false;
	if(getSize() > field.getSize()) return true;
	if(getSize() < field.getSize()) return false;

	return strcmp(m_name, field.m_name) < 0;
}

//-------------------------------------------------------------------
bool Field::isVector(void) const {
	const size_t cnt = m_sub.size();
	
	if(cnt >= 2 && cnt <= 4) {
		const field::Type type = m_sub.front().getType();
		
		if(type == field::Type::STRUCT)
			return false;

		for(const Field& field : m_sub) {
			if(field.isDimensional())
				return false;

			if(field.getType() != type)
				return false;
		}

		return true;
	}

	return false;
}

//-------------------------------------------------------------------
bool Field::isMultiDimensional(void) const {
	return getDimCount() > 1;
}

//-------------------------------------------------------------------
uint32_t Field::getDimCount(void) const {
	return (uint32_t)m_counts.size();
}

//-------------------------------------------------------------------
uint32_t Field::getVDimCount(void) const {
	if(isCube())
		return 1;

	uint32_t i = 0;
	for(uint32_t c : m_counts)
		if(c == 0) i++;
	return i;
}

//-------------------------------------------------------------------
uint32_t Field::getTranspositionCount(void) const {
	return (uint32_t)m_indexes.size();
}

//-------------------------------------------------------------------
field::Type Field::getType(void) const {
	return m_type;
}

//-------------------------------------------------------------------
const std::list<Field>& Field::getChildren(void) const {
	return m_sub;
}

//-------------------------------------------------------------------
std::list<Field>& Field::getChildren(void) {
	return m_sub;
}

//-------------------------------------------------------------------
void Field::sort(void) {
	if(hasChildren()) {
		for(Field& field : m_sub)
			field.sort();
	}
	m_sub.sort();
}

//-------------------------------------------------------------------
bool Field::isStruct(void) const {
	return getType() == field::Type::STRUCT;
}

//-------------------------------------------------------------------
bool Field::isLayoutStruct(void) const {
	return isStruct() && m_sub.size() > 1 && isDimensional();
}

//-------------------------------------------------------------------
bool Field::isDimensional(void) const {
	return getDimCount() > 0;
}

//-------------------------------------------------------------------
bool Field::isAOSOA(void) const {
	assert(isStruct());
	return isRoot();
}

//-------------------------------------------------------------------
void Field::setGlobalId					(const uint32_t id) { m_globalId = id; }
void Field::setGlobalLayoutId			(const uint32_t id) { m_globalLayoutId = id; }
void Field::setGlobalTranspositionId	(const uint32_t id) { m_globalTranspositionId = id; }
void Field::setGlobalMemoryId			(const uint32_t id) { m_globalMemoryId = id; }

//-------------------------------------------------------------------
uint32_t Field::getGlobalId				(void) const { return m_globalId; }
uint32_t Field::getGlobalLayoutId		(void) const { return m_globalLayoutId; }
uint32_t Field::getGlobalTranspositionId(void) const { return m_globalTranspositionId; }
uint32_t Field::getGlobalMemoryId		(void) const { return m_globalMemoryId; }

//-------------------------------------------------------------------
void Field::setSharedId					(const uint32_t id) { m_sharedId = id; }
void Field::setSharedLayoutId			(const uint32_t id) { m_sharedLayoutId = id; }
void Field::setSharedTranspositionId	(const uint32_t id) { m_sharedTranspositionId = id; }

//-------------------------------------------------------------------
uint32_t Field::getSharedId				(void) const { return m_sharedId; }
uint32_t Field::getSharedLayoutId		(void) const { return m_sharedLayoutId; }
uint32_t Field::getSharedTranspositionId(void) const { return m_sharedTranspositionId; }

//-------------------------------------------------------------------
void Field::setDynId					(const uint32_t id) { m_dynId = id; }
void Field::setDynLayoutId				(const uint32_t id) { m_dynLayoutId = id; }
void Field::setDynTranspositionId		(const uint32_t id) { m_dynTranspositionId = id; }

//-------------------------------------------------------------------
uint32_t Field::getDynId				(void) const { return m_dynId; }
uint32_t Field::getDynLayoutId			(void) const { return m_dynLayoutId; }
uint32_t Field::getDynTranspositionId	(void) const { return m_dynTranspositionId; }

//-------------------------------------------------------------------
bool Field::isCube(void) const {
	return m_isCube;
}

//-------------------------------------------------------------------
const std::vector<uint32_t>& Field::getCounts(void) const {
	assert(isDimensional());
	return m_counts;
}

//-------------------------------------------------------------------
const char* Field::getTypeName(void) const {
	// do not test for isStruct here, as it also checks the number of children.
	if(getType() == field::Type::STRUCT)
		return getName();

	return array::field::getTypeString(getType());
}

//-------------------------------------------------------------------
const char* Field::getVTypeName(void) const {
	assert(isStruct());
	assert(isVector());
	assert(!m_sub.empty());

	if(m_sub.size() == 2) {
		switch(m_sub.front().getType()) {
		case field::Type::FLOAT:	return "float2";
		case field::Type::DOUBLE:	return "doubl2";
		case field::Type::S8:		return "char2";
		case field::Type::S16:		return "short2";
		case field::Type::S32:		return "int2";
		case field::Type::S64:		return "longlong2";
		case field::Type::U8:		return "uchar2";
		case field::Type::U16:		return "ushort2";
		case field::Type::U32:		return "uint2";
		case field::Type::U64:		return "ulonglong2";
		default:;
		}
	} else if(m_sub.size() == 3) {
		switch(m_sub.front().getType()) {
		case field::Type::FLOAT:	return "float3";
		case field::Type::DOUBLE:	return "double3";
		case field::Type::S8:		return "char3";
		case field::Type::S16:		return "short3";
		case field::Type::S32:		return "int3";
		case field::Type::S64:		return "longlong3";
		case field::Type::U8:		return "uchar3";
		case field::Type::U16:		return "ushort3";
		case field::Type::U32:		return "uint3";
		case field::Type::U64:		return "ulonglong3";
		default:;
		}
	} else {
		switch(m_sub.front().getType()) {
		case field::Type::FLOAT:	return "float4";
		case field::Type::DOUBLE:	return "double4";
		case field::Type::S8:		return "char4";
		case field::Type::S16:		return "short4";
		case field::Type::S32:		return "int4";
		case field::Type::S64:		return "longlong4";
		case field::Type::U8:		return "uchar4";
		case field::Type::U16:		return "ushort4";
		case field::Type::U32:		return "uint4";
		case field::Type::U64:		return "ulonglong4";
		default:;
		}
	}

	THROW("unknown type");
	return 0;
}

//-------------------------------------------------------------------
void Field::setIndexType(const field::Type internalIndexType, const field::Type externalIndexType) {
	m_internalIndexType = internalIndexType;
	m_externalIndexType = externalIndexType;
}

//-------------------------------------------------------------------
field::Type Field::getInternalIndexType(void) const {
	return m_internalIndexType;
}

//-------------------------------------------------------------------
field::Type Field::getExternalIndexType(void) const {
	return m_externalIndexType;
}

//-------------------------------------------------------------------
bool Field::isPrimitive(void) const {
	assert(isStruct());

	for(const auto& sub : getChildren()) {
		if(sub.isStruct())
			return false;
	}

	return true;
}

//-------------------------------------------------------------------
uint32_t Field::getElementCount(void) const {
	uint32_t cnt = 1;
	for(const uint32_t value : m_counts)
		cnt *= value;

	return cnt;
}

//-------------------------------------------------------------------
bool Field::isCustomCount(void) const {
	return m_customCount != 0;
}

//-------------------------------------------------------------------
const char* Field::getCustomCount(void) const {
	assert(getDimCount() > 0);
	return m_customCount;
}

//-------------------------------------------------------------------
void Field::setCustomCount(const char* equation) {
	assert(getDimCount() > 0);
	assert(!m_customCount);
	m_customCount = NEW_STRING(equation);
}

//-------------------------------------------------------------------
const std::list<const char*>& Field::getIndex(void) const {
	assert(getDimCount() > 0);
	return m_indexes;
}

//-------------------------------------------------------------------
void Field::addIndex(const char* index) {
	assert(getDimCount() > 0);
	m_indexes.emplace_back(NEW_STRING(index));
}

//-------------------------------------------------------------------
bool Field::isSubArrays(void) const {
	assert(isStruct());

	for(const Field& sub : m_sub) {
		if(sub.isStruct())
			return true;
	}

	return false;
}

//-------------------------------------------------------------------
bool Field::isConstant(void) const {
	if(getParent() != 0)
		return false;

	for(const auto& cnt : m_counts) {
		if(cnt == 0)
			return false;
	}

	return true;
}

//-------------------------------------------------------------------
	}
}