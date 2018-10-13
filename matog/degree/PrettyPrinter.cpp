// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "PrettyPrinter.h"
#include <sstream>

namespace matog {
	namespace degree {
//-------------------------------------------------------------------
const char* PrettyPrinter::printLong(const Layout layout) {
	switch(layout) {
		case Layout::AOS:		return "AOS";
		case Layout::SOA:		return "SOA";
		case Layout::AOSOA:		return "AOSOA";
		case Layout::NONE:
		default:				return "-";
	}
}

//-------------------------------------------------------------------
char PrettyPrinter::printShort(const Layout layout) {
	switch(layout) {
		case Layout::AOS:		return 'A';
		case Layout::SOA:		return 'S';
		case Layout::AOSOA:		return 'H';
		case Layout::NONE:
		default:				return '-';
	}
}

//-------------------------------------------------------------------
const char* PrettyPrinter::printLong(const Memory memory) {
	switch(memory) {
		case Memory::GLOBAL:		return "Global";
		case Memory::TEXTURE:		return "Texture";
		case Memory::CONSTANT:		return "Constant";
		case Memory::NONE:
		default:					return "-";
	}
}

//-------------------------------------------------------------------
char PrettyPrinter::printShort(const Memory memory) {
	switch(memory) {
		case Memory::GLOBAL:		return 'G';
		case Memory::TEXTURE:		return 'T';
		case Memory::CONSTANT:		return 'C';
		case Memory::NONE:
		default:					return '-';
	}
}
	                             	                     
//-------------------------------------------------------------------      
char PrettyPrinter::printShort(const L1Cache cache) {
	switch(cache) {
		case L1Cache::SM:	return 'S';
		case L1Cache::L1:	return 'L';
		case L1Cache::EQ:	return 'E';
		default:			return '-';
	}
}

//-------------------------------------------------------------------
const char* PrettyPrinter::printLong(const L1Cache cache) {
	switch(cache) {
		case L1Cache::SM:	return "Shared Memory";
		case L1Cache::L1:	return "L1 Cache";
		case L1Cache::EQ:	return "Equal";
		default:			return "default";
	}
}

//-------------------------------------------------------------------
	}
}