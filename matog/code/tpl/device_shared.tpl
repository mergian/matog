// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file. 
#ifndef __{{GLOBAL_NAME:x-uppercase}}
#define __{{GLOBAL_NAME:x-uppercase}}

#include <cstdint>
#include <cassert>
#include <type_traits>

#ifndef THROW
#define THROW() assert(0); __trap()
#endif

#ifdef __matog_inline__
#undef __matog_inline__
#endif

#define __matog_inline__ __device__ __forceinline__

{{#IS_TEMPLATE}}template<{{>INDEX}}>{{/IS_TEMPLATE}}
class {{GLOBAL_NAME}} {
private:
	// UNION ----------------------------------------------------
	{{>UNION}}

	// UNION ----------------------------------------------------
	
	// POINTER --------------------------------------------------
	{{>POINTER}}
	// POINTER --------------------------------------------------

	/// format getter
	{{>FORMAT}}
					
public:
	/// default constructor
	__device__ {{GLOBAL_NAME}}(void) {};
	
	/// no copy allowed
	__device__ {{GLOBAL_NAME}}(const {{GLOBAL_NAME}}&) = delete;
	
	/// element counts
	{{>ELEMENT_COUNTS}}
	
	/// counts
	{{>COUNTS}}
	
	/// ptr
	{{>PTR}}
	
	/// getter/setter
	{{>GETTER_SETTER}}
	
	/// accessor
	{{>ACCESSOR_FORWARD_DECLARATION}}
	{{>ACCESSOR_DATA}}
	{{>ACCESSOR}}
};

#endif