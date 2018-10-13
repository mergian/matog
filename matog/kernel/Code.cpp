// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Code.h"
#include <matog/log.h>
#include <matog/util.h>

using namespace matog::util;

namespace matog {
	namespace kernel {
//-------------------------------------------------------------------
bool Code::charsToRemove(char c)
{
    switch(c)
    {
    case ' ':	
	case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return true;
    default:
        return false;
    }
}


//-------------------------------------------------------------------
String Code::toPreprocessor(const char* in) {
	return String(in).removeIf(charsToRemove);
}

//-------------------------------------------------------------------
String Code::define(const char* name) {
	std::ostringstream ss;
	ss << "__MATOG__";
	
	for(; *name != 0; name++) {
		if(*name >= 'a' && *name <= 'z')
			ss << (char)(*name + 'A' - 'a');
		else
			ss << *name;
	}

	return ss.str();
}

//-------------------------------------------------------------------
void Code::define(std::ostream& ss, const char* name) {
	ss << "__MATOG__"; 

	for(; *name != 0; name++) {
		if(*name >= 'a' && *name <= 'z')
			ss << (char)(*name + 'A' - 'a');
		else
			ss << *name;
	}
}

//-------------------------------------------------------------------
String Code::layoutDefine(const char* name) {
	std::ostringstream ss;
	ss << define(name) << "_L";
	return ss.str();
}

//-------------------------------------------------------------------
String Code::memoryDefine(const char* name) {
	std::ostringstream ss;
	ss << define(name) << "_M";
	return ss.str();
}

//-------------------------------------------------------------------
String Code::transpositionDefine(const char* name) {
	std::ostringstream ss;
	ss << define(name) << "_T";
	return ss.str();
}

//-------------------------------------------------------------------
String Code::ifndef(const char* name) {
	std::ostringstream ss;
	ss << define(name) << "_CU";
	return ss.str();
}

//-------------------------------------------------------------------
const char*	Code::texFormat(const bool isFloat, const bool isSigned, const uint32_t size) {
	if(isFloat) {
		if(size == 4) // FLOAT
			return "CU_AD_FORMAT_FLOAT";
	} else if(isSigned) {
		switch(size) {
			case 1: return "CU_AD_FORMAT_SIGNED_INT8";
			case 2: return "CU_AD_FORMAT_SIGNED_INT16";
			case 4: return "CU_AD_FORMAT_SIGNED_INT32";
		};
 	} else {
		switch(size) {
			case 1: return "CU_AD_FORMAT_UNSIGNED_INT8";
			case 2: return "CU_AD_FORMAT_UNSIGNED_INT16";
			case 4: return "CU_AD_FORMAT_UNSIGNED_INT32";
		};
 	}

	THROW("UNKNOWN_TEXTURE_FORMAT");
	return 0;	
}

//-------------------------------------------------------------------
const char*	Code::texFlag(const bool isFloat) {
	if(isFloat)
		return "0";	
	return "CU_TRSF_READ_AS_INTEGER";
}

//-------------------------------------------------------------------
	}
}