// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/runtime/MemArray.h>
#include <matog/Array.h>

using matog::Array;

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
MemArray::MemArray(const uint32_t arrayId, const bool isDevice, const void* itemPtr, const std::initializer_list<uint64_t>& counts, const char* file, const int line) :
	BaseArray(arrayId, isDevice, itemPtr, counts, file, line)
	// do not initialize vars here, as they are initialized before!
{}

//-------------------------------------------------------------------
MemArray::~MemArray(void) {
}

//-------------------------------------------------------------------
	}
}