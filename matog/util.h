// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_UTIL
#define __MATOG_UTIL

#include <matog/util/dll.h>
#include <cstdint>
#include <cstddef>

namespace matog {
	namespace util {
		struct blob {
			char* ptr;
			size_t size;

			inline blob(void) : ptr(0), size(0) {}
			inline blob(void* _ptr, size_t _size) : ptr((char*)_ptr), size(_size) {}
			inline blob(const void* _ptr, size_t _size) : ptr((char*)_ptr), size(_size) {}
		};

		class MATOG_UTIL_DLL Env;
		class MATOG_UTIL_DLL Exit;
		class MATOG_UTIL_DLL Hash;
		class MATOG_UTIL_DLL IO;
		class MATOG_UTIL_DLL Mem;
		class MATOG_UTIL_DLL Random;
		class MATOG_UTIL_DLL SQLiteDB;
		class MATOG_UTIL_DLL SQLStmt;
		class MATOG_UTIL_DLL String;
		class MATOG_UTIL_DLL String;
		
		namespace ml {
			class MATOG_UTIL_DLL Direction;
			class MATOG_UTIL_DLL Euclidean;
			class MATOG_UTIL_DLL Model;
			class MATOG_UTIL_DLL None;
			class MATOG_UTIL_DLL SVM;
			enum class Type;
			
			extern Type MATOG_UTIL_DLL getTypeByName(const char* name);
		}
	}
}
		
#endif