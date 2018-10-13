// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_VARIANT_TYPE
#define __MATOG_VARIANT_TYPE

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
enum class Type {
	Global = 0,			//!< only global
	Local,				//!< only local
	Shared,				//!< only shared
	Independent,		//!< only indepedent
	LocalIndependent,	//!< local and independent
	Module,				//!< only module
	Compiler,			//!< only compiler
	All,				//!< All degrees
	CNT					//!< dummy to get number of types
};

//-------------------------------------------------------------------
	}
}

#endif