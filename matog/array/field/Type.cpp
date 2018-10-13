// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/internal.h>
#include "Type.h"
#include <matog/log.h>

#define IF(NAME) if(strcmp(name, NAME) == 0)

namespace matog {
	namespace array {
		namespace field {
//-------------------------------------------------------------------
MATOG_INTERNAL_DLL Type getTypeByName(const char* name) {
	THROWIF(strcmp(name, "bool") == 0, "MATOG does not support bool types, as these are 32bit on CUDA. Use char instead.");

	IF("char")						return Type::S8;
	IF("signed cha")				return Type::S8;
	IF("short")						return Type::S16;
	IF("signed short")				return Type::S16;
	IF("int")						return Type::S32;
	IF("signed")					return Type::S32;
	IF("signed int")				return Type::S32;
	IF("long")						return Type::S64;
	IF("signed long")				return Type::S64;
	IF("signed long long")			return Type::S64;
	IF("signed long int")			return Type::S64;
	IF("signed long long int")		return Type::S64;

	IF("unsigned char")				return Type::U8;
	IF("unsigned short")			return Type::U16;
	IF("unsigned")					return Type::U32;
	IF("unsigned int")				return Type::U32;
	IF("unsigned long")				return Type::U64;
	IF("unsigned long long")		return Type::U64;
	IF("unsigned long int")			return Type::U64;
	IF("unsigned long long int")	return Type::U64;

	IF("int8_t")					return Type::S8;
	IF("int16_t")					return Type::S16;
	IF("int32_t")					return Type::S32;
	IF("int64_t")					return Type::S64;
	IF("uint8_t")					return Type::U8;
	IF("uint16_t")					return Type::U16;
	IF("uint32_t")					return Type::U32;
	IF("uint64_t")					return Type::U64;

	IF("float")						return Type::FLOAT;
	IF("double")					return Type::DOUBLE;
	
	IF("struct")					return Type::STRUCT;

	THROW("Unknown field type");
	return Type::UNKNOWN;
}

//-------------------------------------------------------------------
MATOG_INTERNAL_DLL uint32_t getTypeSize(const Type type) {
	switch(type) {
	case field::Type::U8:
	case field::Type::S8:
		return 1;
	case field::Type::U16:
	case field::Type::S16:
		return 2;
	case field::Type::U32:
	case field::Type::S32:
	case field::Type::FLOAT:
		return 4;
	case field::Type::U64:
	case field::Type::S64:
	case field::Type::DOUBLE:
		return 8;
	default: 
		;
	};
	
	THROWIF("Unknown field type.");
	return 0;
}

//-------------------------------------------------------------------
MATOG_INTERNAL_DLL const char* getTypeString(const Type type) {
	switch(type) {
	case field::Type::U8:		return "uint8_t";
	case field::Type::S8:		return "int8_t";
	case field::Type::U16:		return "uint16_t";
	case field::Type::S16:		return "int16_t";
	case field::Type::U32:		return "uint32_t";
	case field::Type::S32:		return "int32_t";
	case field::Type::FLOAT:	return "float";
	case field::Type::U64:		return "uint64_t";
	case field::Type::S64:		return "int64_t";
	case field::Type::DOUBLE:	return "double";
	default: 
		;
	};
	
	THROWIF("Unknown field type.");
	return 0;
}

//-------------------------------------------------------------------
MATOG_INTERNAL_DLL const char* getTexTypeString(const uint32_t size) {
	switch(size) {
	case 1:	return "int8_t";
	case 2:	return "int16_t";
	case 4:	return "int32_t";
	case 8:	return "int2";
	default: 
		;
	};

	THROWIF("Unknown field type.");
	return 0;
}

//-------------------------------------------------------------------
MATOG_INTERNAL_DLL const char* getTexTypeString(const Type type) {
	return getTexTypeString(getTypeSize(type));
}

//-------------------------------------------------------------------
		}
	}
}