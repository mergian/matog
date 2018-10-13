// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_MEMARRAY
#define __MATOG_RUNTIME_MEMARRAY

#include <matog/runtime/BaseArray.h>

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
/**
	Abstract array implementation for global memory arrays
*/
class MATOG_RUNTIME_DLL MemArray : public BaseArray {
protected:
	uint64_t	m_mem_0;	///< internally used
	uint64_t	m_size;		///< size

	/// Default Constructor
	MemArray(const uint32_t arrayId, const bool isDevice, const void* const itemPtr, const std::initializer_list<uint64_t>& counts, const char* file, const int line);

public:
	/// Default Destructor
	virtual				~MemArray		(void);

	/// gets the total size of this array
	inline uint64_t		size			(void) const { return m_size; }

	/// gets the total valid size of this array
	virtual uint64_t	copy_size		(void) const = 0;
};

//-------------------------------------------------------------------
	}
}

#endif