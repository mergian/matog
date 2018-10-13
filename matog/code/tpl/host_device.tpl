// Copyright (c) 2016 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file. 
#include "{{GLOBAL_NAME}}.h"
#include <matog/log.h>

//-------------------------------------------------------------------
{{GLOBAL_NAME}}::Device::Device({{#IS_DYN_COUNT}}{{>DEVICE_INDEX}}, {{/IS_DYN_COUNT}}const char* file, const int line, matog::runtime::DeviceMalloc* allocator) :
	matog::runtime::DeviceArray({{GLOBAL_ID}}, __init_counts({{>VALUES}}), {{{>VALUES}}}, file, line, allocator)
{
	__init_allocation();
	__init_ptr();
}

//-------------------------------------------------------------------
{{GLOBAL_NAME}}::Device::~Device(void) {
}

//-------------------------------------------------------------------
void {{GLOBAL_NAME}}::Device::__init_allocation(void) {
	allocator()->malloc(&m_dataPtr, m_size);
}

//-------------------------------------------------------------------
void* {{GLOBAL_NAME}}::Device::__init_counts({{>DEVICE_INDEX}}) {
	{{#COUNTS}}THROWIF({{CHAR}} == 0, "Index {{CHAR}} of {{GLOBAL_NAME}}::Device has to be non-zero!");
	{{/COUNTS}}
	{{#COUNTS}}m_cnt[{{DIM}}] = {{CHAR}};
	{{/COUNTS}}
	m_size = calc_size({{>VALUES}});
	
	return &m_ptr;
}

//-------------------------------------------------------------------
void {{GLOBAL_NAME}}::Device::__init_ptr(void) {
	{{>DEVICE_INIT_PTR}}
}

//-------------------------------------------------------------------
matog::runtime::DeviceArray* {{GLOBAL_NAME}}::Device::clone(const char* file, const int line, matog::runtime::DeviceMalloc* malloc) const {
	return new Device({{#COUNTS}}m_cnt[{{DIM}}], {{/COUNTS}}file, line, malloc);
}

//-------------------------------------------------------------------
matog::runtime::HostArray* {{GLOBAL_NAME}}::Device::clone_host(const char* file, const int line, matog::runtime::HostMalloc* malloc) const {
	return new Host({{#COUNTS}}m_cnt[{{DIM}}], {{/COUNTS}}file, line, malloc);
}

//-------------------------------------------------------------------
uint64_t {{GLOBAL_NAME}}::Device::copy_size(void) const {
	{{>GET_COPY_SIZE}}
}