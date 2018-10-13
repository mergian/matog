// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_DYNARRAY
#define __MATOG_RUNTIME_DYNARRAY

#include <matog/runtime/BaseArray.h>

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
/**
	Base Dynamic Shared array class
*/
class MATOG_RUNTIME_DLL DynArray : public BaseArray {
protected:
	uint32_t m_dyn_0; ///< internal use only
	uint32_t m_dyn_1; ///< internal use only

	/// Default Constructor
	DynArray(const uint32_t arrayId, const void* itemPtr, const std::initializer_list<uint64_t>& counts, const char* file, const int line);

	/// internal use only
	virtual uint32_t __dyn_0(const uint8_t) const = 0;

	/// internal use only
	virtual uint32_t __dyn_1(void) const = 0;

public:				
	/// Default Destructor
	virtual	~DynArray(void);
};

//-------------------------------------------------------------------
	}
}

#endif