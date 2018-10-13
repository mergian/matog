// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_UTIL_HASH_H
#define __MATOG_UTIL_HASH_H

#include "dll.h"
#include <cstdint>
#include <cstdlib>

namespace matog {
	namespace util {
//-------------------------------------------------------------------
/// Helper class to calculate CRC32 hashes
class MATOG_UTIL_DLL Hash {
public:
	/// calculate CRC32 of string
	static uint32_t	crc32		(const char* in, size_t len = 0);

	/// init CRC32 calculation
	static uint32_t	crc32_init	(void);

	/// step CRC32 8Bit
	static uint32_t	crc32_step	(const uint32_t crc, const uint8_t data);

	/// step CRC32 16Bit
	static uint32_t	crc32_step16(uint32_t crc, const uint16_t data);

	/// step CRC32 32Bit
	static uint32_t	crc32_step32(uint32_t crc, const uint32_t data);

	/// step CRC32 64Bit
	static uint32_t	crc32_step64(uint32_t crc, const uint64_t data);

	/// finalize CRC32
	static uint32_t	crc32_final	(const uint32_t crc);

	/// create CRC32 Hex String
	static const char* crc32s	(const char* in, const size_t len = 0);
};

//-------------------------------------------------------------------
	}
}

#endif