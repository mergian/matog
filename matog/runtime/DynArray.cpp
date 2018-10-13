// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/runtime/DynArray.h>

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
DynArray::DynArray(const uint32_t arrayId, const void* itemPtr, const std::initializer_list<uint64_t>& counts, const char* file, const int line) :
	BaseArray(arrayId, false, itemPtr, counts, file, line) 
{}

//-------------------------------------------------------------------
DynArray::~DynArray(void) {
}

//-------------------------------------------------------------------
	}
}