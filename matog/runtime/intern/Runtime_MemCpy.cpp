// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Runtime.h"
#include "DeviceArray.h"
#include "HostArray.h"
#include "exec/Profiler.h"
#include <matog/log.h>

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
using exec::Profiler;

//-------------------------------------------------------------------
void Runtime::Memcpy(DeviceArray* const dest, const DeviceArray* const src) {
	MemcpyDtoD(dest, src);
}

//-------------------------------------------------------------------
void Runtime::MemcpyAsync(DeviceArray* const dest, const DeviceArray* const src, CUstream hStream) {
	MemcpyDtoDAsync(dest, src, hStream);
}

//-------------------------------------------------------------------
void Runtime::MemcpyDtoD(DeviceArray* const dest, const DeviceArray* const src) {
	THROWIF(dest->getArray().getId() != src->getArray().getId(), "COPY_ERROR", "Copy src and dest have to be of the same array type!");
	assert(src->getConfig() != UINT64_MAX);	

	Context::get().copyDecision((MemArray<>*)dest, (MemArray<>*)src);
	CHECK(cuMemcpyDtoD(dest->data_ptr(), src->data_ptr(), dest->copy_size()));
}

//-------------------------------------------------------------------
void Runtime::MemcpyDtoDAsync(DeviceArray* const dest, const DeviceArray* const src, CUstream hStream) {
	THROWIF(dest->getArray().getId() != src->getArray().getId(), "COPY_ERROR", "Copy src and dest have to be of the same array type!");
	assert(src->getConfig() != UINT64_MAX);

	Context::get().copyDecision((MemArray<>*)dest, (MemArray<>*)src);
	CHECK(cuMemcpyDtoDAsync(dest->data_ptr(), src->data_ptr(), dest->copy_size(), hStream));
}

//-------------------------------------------------------------------
void Runtime::MemcpyDtoH(HostArray* const dest, const DeviceArray* const src) {
	THROWIF(dest->getArray().getId() != src->getArray().getId(), "COPY_ERROR", "Copy src and dest have to be of the same array type!");
	assert(src->getConfig() != UINT64_MAX);

	Context::get().copyDecision((MemArray<>*)dest, (MemArray<>*)src);
	CHECK(cuMemcpyDtoH(dest->data_ptr(), src->data_ptr(), dest->copy_size()));
}

//-------------------------------------------------------------------
void Runtime::MemcpyDtoHAsync(HostArray* const dest, const DeviceArray* const src, CUstream hStream) {
	THROWIF(dest->getArray().getId() != src->getArray().getId(), "COPY_ERROR", "Copy src and dest have to be of the same array type!");
	assert(src->getConfig() != UINT64_MAX);

	Context::get().copyDecision((MemArray<>*)dest, (MemArray<>*)src);
	CHECK(cuMemcpyDtoHAsync(dest->data_ptr(), src->data_ptr(), dest->copy_size(), hStream));
}

//-------------------------------------------------------------------
void Runtime::MemcpyHtoD(DeviceArray* const dest, const HostArray* const src) {
	THROWIF(dest->getArray().getId() != src->getArray().getId(), "COPY_ERROR", "Copy src and dest have to be of the same array type!");
	assert(src->getConfig() != UINT64_MAX);

	Context::get().copyDecision((MemArray<>*)dest, (MemArray<>*)src);
	CHECK(cuMemcpyHtoD(dest->data_ptr(), src->data_ptr(), dest->copy_size()));
}

//-------------------------------------------------------------------
void Runtime::MemcpyHtoDAsync(DeviceArray* const dest, const HostArray* const src, CUstream hStream) {
	THROWIF(dest->getArray().getId() != src->getArray().getId(), "COPY_ERROR", "Copy src and dest have to be of the same array type!");
	assert(src->getConfig() != UINT64_MAX);

	Context::get().copyDecision((MemArray<>*)dest, (MemArray<>*)src);
	CHECK(cuMemcpyHtoDAsync(dest->data_ptr(), src->data_ptr(), dest->copy_size(), hStream));
}

//-------------------------------------------------------------------
		}
	}
}