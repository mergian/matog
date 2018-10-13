// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_BASEARRAY
#define __MATOG_RUNTIME_BASEARRAY

#include <matog/runtime/dll.h>
#include <matog/enums.h>
#include <matog/runtime/Malloc.h>
#include <initializer_list>

// used for debugging purposes
#define _fl __FILE__, __LINE__

namespace matog {
	namespace runtime {
//-------------------------------------------------------------------
/**
	Base Array Class definition for all array implementations
*/
class MATOG_RUNTIME_DLL BaseArray {
protected:
	const	char* const			m_file;			///< file in which the array was allocated
	const	int					m_line;			///< line in which the array was allocated

	const	void* const			m_array_0;		///< internal use only
	const	void* 				m_array_1;		///< internal use only
			uint32_t			m_array_2;		///< internal use only
			uint32_t			m_array_3;		///< internal use only
			uint32_t			m_array_4;		///< internal use only
			uint32_t			m_array_5;		///< internal use only
			uint32_t			m_array_6;		///< internal use only
			uint64_t*			m_array_7;		///< internal use only

	/// Default Constructor
	BaseArray(const uint32_t arrayId, const bool isDevice, const void* const itemPtr, const std::initializer_list<uint64_t>& counts, const char* file, const int line);

public:
	/// Destructor
	virtual ~BaseArray(void);

	/// returns the file in which the array was allocated
	inline	const char*	file(void) const { return m_file; }

	/// returns the line in which the array was allocated
	inline	const int	line(void) const { return m_line; }
	
	/// helper function to enforce a specific alignment for texture memory
	static uint64_t textureAlign(uint64_t a);

	/// helper function to enforce a specific alignment
	static uint64_t align(uint64_t a, size_t alignment = sizeof(uint64_t));

	/// helper function to enforce a specific alignment
	static uint32_t align(uint32_t a, size_t alignment = sizeof(uint64_t));
};

//-------------------------------------------------------------------
	}
}

#endif