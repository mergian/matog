// Copyright (c) 2016 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file. 
#include "{{GLOBAL_NAME}}.h"
#include <matog/log.h>
#include <algorithm>

{{#IS_STRUCT}}//-------------------------------------------------------------------
static uint64_t align(uint64_t value) {
	return matog::runtime::BaseArray::align(value);
}{{/IS_STRUCT}}

//-------------------------------------------------------------------
{{GLOBAL_NAME}}::Host::Host({{#IS_DYN_COUNT}}{{>INDEX}}, {{/IS_DYN_COUNT}}const char* file, const int line, matog::runtime::HostMalloc* allocator) :
	matog::runtime::HostArray({{GLOBAL_ID}}, __init_counts({{>VALUES}}), {{{>VALUES}}}, file, line, allocator)
{
	__init_allocation();
	__init_ptr();
}

//-------------------------------------------------------------------
{{GLOBAL_NAME}}::Host::~Host(void) {
}

//-------------------------------------------------------------------
void {{GLOBAL_NAME}}::Host::__init_allocation(void) {
	allocator()->malloc(&m_dataPtr, m_size);
}

//-------------------------------------------------------------------
void* {{GLOBAL_NAME}}::Host::__init_counts({{>INDEX}}) {
	{{#COUNTS}}THROWIF({{CHAR}} == 0, "Index {{CHAR}} of {{GLOBAL_NAME}}::Host has to be >0!");
	{{/COUNTS}}
	{{#COUNTS}}m_cnt[{{DIM}}] = {{CHAR}};
	{{/COUNTS}}
	m_size = calc_size({{>VALUES}});
	
	return &m_ptr;
}

//-------------------------------------------------------------------
void {{GLOBAL_NAME}}::Host::__init_ptr(void) {
	{{>HOST_INIT_PTR}}
}

//-------------------------------------------------------------------
matog::runtime::HostArray* {{GLOBAL_NAME}}::Host::clone(const char* file, const int line, matog::runtime::HostMalloc* malloc) const {
	return new Host({{#COUNTS}}m_cnt[{{DIM}}], {{/COUNTS}}file, line, malloc);
}

//-------------------------------------------------------------------
matog::runtime::DeviceArray* {{GLOBAL_NAME}}::Host::clone_device(const char* file, const int line, matog::runtime::DeviceMalloc* malloc) const {
	return new Device({{#COUNTS}}({{DEVICE_INDEX_TYPE}})m_cnt[{{DIM}}], {{/COUNTS}}file, line, malloc);
}

//-------------------------------------------------------------------
void {{GLOBAL_NAME}}::Host::copy(const matog::runtime::HostArray* source) {
	const Host& src = *reinterpret_cast<const Host*>(source);
	Host& dest = *this;
	{{>COPY}}
}

//-------------------------------------------------------------------
bool {{GLOBAL_NAME}}::Host::compare(const matog::runtime::HostArray* source) {
	const Host& src = *reinterpret_cast<const Host*>(source);
	Host& dest = *this;
	{{>COMPARE}}
	return true;
}

//-------------------------------------------------------------------
uint64_t {{GLOBAL_NAME}}::Host::copy_size(void) const {
	{{>GET_COPY_SIZE}}
}

//-------------------------------------------------------------------
uint64_t {{GLOBAL_NAME}}::calc_size_by_elements(const uint64_t cnt) {
	{{>CALC_SIZE_BY_ELEMENTS}}
}

//-------------------------------------------------------------------
uint64_t {{GLOBAL_NAME}}::calc_size({{>INDEX}}) {
	{{>CALC_SIZE}}
}