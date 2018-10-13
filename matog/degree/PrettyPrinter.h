// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DEGREE_PRETTYPRINTER
#define __MATOG_DEGREE_PRETTYPRINTER

#include <matog/dll.h>
#include <matog/enums.h>
#include <cuda.h>

namespace matog {
	namespace degree {
//-------------------------------------------------------------------
/**
	Helper class to translate Enums to strings or chars
*/
class MATOG_INTERNAL_DLL PrettyPrinter {
public:
	/// returns string of Layout
	static const char*	printLong	(const Layout layout);

	/// returns char of Layout
	static char			printShort	(const Layout layout);
	
	/// returns string of Memory
	static const char*	printLong	(const Memory memory);

	/// returns char of Memory
	static char			printShort	(const Memory memory);
                          	 
	/// returns string of L1cache (SM, L1 or EQ)
	static const char*	printLong	(const L1Cache cache);

	/// returns char of L1cache (S, L or M)
	static char			printShort	(const L1Cache cache);
};
//-------------------------------------------------------------------
	}
}

#endif