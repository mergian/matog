// Copyright (c) 2016 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file. 
#include "{{GLOBAL_NAME}}.h"
#include <matog/log.h>

//-------------------------------------------------------------------
{{GLOBAL_NAME}}::Dyn::Dyn({{#IS_DYN_COUNT}}{{>DYN_INDEX}}, {{/IS_DYN_COUNT}}const char* file, const int line) :
	matog::runtime::DynArray({{DYN_ID}}, __init_counts({{>VALUES}}), {{{>VALUES}}}, file, line)
{}

//-------------------------------------------------------------------
{{GLOBAL_NAME}}::Dyn::~Dyn(void) {
}

//-------------------------------------------------------------------
void* {{GLOBAL_NAME}}::Dyn::__init_counts({{>DYN_INDEX}}) {
	{{#COUNTS}}THROWIF({{CHAR}} == 0, "Index {{CHAR}} of {{GLOBAL_NAME}}::Dyn has to be non-zero!");
	{{/COUNTS}}
	{{#COUNTS}}m_cnt[{{DIM}}] = {{CHAR}};
	{{/COUNTS}}

	return (void*)&m_dyn_1;
}

//-------------------------------------------------------------------
uint32_t {{GLOBAL_NAME}}::Dyn::__dyn_0(const uint8_t layout) const {
	{{>DYN_0}}
}

//-------------------------------------------------------------------
uint32_t {{GLOBAL_NAME}}::Dyn::__dyn_1(void) const {
	return {{DYN_ALIGNMENT}};
}