// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_KERNEL_CODE
#define __MATOG_KERNEL_CODE

#include <matog/dll.h>
#include <matog/util/String.h>

namespace matog {
	namespace kernel {
//-------------------------------------------------------------------

/// Helper class that provides methods for the code generation
class MATOG_INTERNAL_DLL Code {
private:
	/// helper for toPreprocessor
	static bool			charsToRemove		(char c);

public:
	/// Removes unallowed chars from string
	static util::String toPreprocessor		(const char* in);

	/// Creates a preprocessor define for a given name
	static util::String define				(const char* name);

	/// Create a preprocessor define for a given name
	static void			define				(std::ostream& ss, const char* name);

	/// Creates a define for optimizing layouts, for a given name
	static util::String layoutDefine		(const char* name);

	/// Creates a define for optimizing memory, for a given name
	static util::String memoryDefine		(const char* name);

	/// Creates a define for optimizing transpositions, for a given name
	static util::String transpositionDefine	(const char* name);

	/// Creates a IFNDEF string for a name
	static util::String ifndef				(const char* name);

	/// Returns the texture format string for a given type
	static const char*	texFormat			(const bool isFloat, const bool isSigned, const uint32_t size);

	/// Returns the texture flag for a given type. 
	/// By Default CUDA casts non-32Bit ints to float!
	static const char*	texFlag				(const bool isFloat);
};

//-------------------------------------------------------------------
	}
}

#endif