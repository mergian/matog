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

extern __shared__ char __{{GLOBAL_NAME:x-uppercase}}_PTR[];

template<uint32_t PID = 0>
class {{GLOBAL_NAME}} {
private:
	// UNION ----------------------------------------------------
	{{>UNION}}
	{{>STRUCT}}
	// UNION ----------------------------------------------------

	// DATA -----------------------------------------------------
	uint32_t m_offset;
	{{#IS_DYN_COUNT}}{{INDEX_TYPE}} m_cnt[{{DIM_COUNT_DYN}}];{{/IS_DYN_COUNT}}
	
	template<typename T> __matog_inline__ T* __ptr(const uint32_t additionalOffset = 0) {
		return reinterpret_cast<T*>(__{{GLOBAL_NAME:x-uppercase}}_PTR + m_offset + additionalOffset);
	}
	
	template<typename T> __matog_inline__ const T* __ptr(const uint32_t additionalOffset = 0) const {
		return reinterpret_cast<const T*>(__{{GLOBAL_NAME:x-uppercase}}_PTR + m_offset + additionalOffset);
	}
	
	// DATA -----------------------------------------------------

	/// format getter
	{{>FORMAT}}
					
public:
	/// no initialization and copy allowed
	__device__ {{GLOBAL_NAME}}(void) = delete;
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