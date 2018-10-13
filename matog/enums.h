// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ENUMS
#define __MATOG_ENUMS

#include <climits>

namespace matog {
	enum class L1Cache {
		SM = 0,
		L1 = 1,
		EQ = 2
	};

	/// Read/Write Mode
	enum class RWMode {
		READ_AND_WRITE	= 0,	///< data will be read and written
		READ_ONLY,				///< data will only be read
		WRITE_ONLY,				///< data will be entirely overwritten by the next kernel call
		READ_ONCE				///< data will be read once and then be written back. requires global sync
	};

	/// Layouts
	enum class Layout {
		SOA = 0,				///< Structure of Arrays
		AOS = 1,				///< Arrays of Structs
		AOSOA = 2,				///< Array of Strucutre of Arrays with 32 Bins
		NONE = INT_MAX
	};

	// Memories
	enum class Memory {
		GLOBAL = 0,				///< Global Memory
		TEXTURE = 1,			///< Texture Memory (CC -eq 3.0) or NC (CC -gt 3.0)
		CONSTANT = 2,			///< Constant Memory
		NONE = INT_MAX
	};
}

#endif