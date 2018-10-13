// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <matog/analyze/graph/Decision.h>
#include <matog/event/Type.h>
#include <matog/Array.h>
#include <matog/Degree.h>
#include <matog/Static.h>
#include <matog/variant/Type.h>

namespace matog {
	namespace analyze {
		namespace graph {
//-------------------------------------------------------------------
Decision::Decision(const uint32_t id, const uint32_t decisionId, const uint32_t arrayId, const uint32_t kernelId, const uint32_t gpuId) : 
	Node			(id, event::Type::Decision), 
	m_finalConfig	(0),
	m_decisionId	(decisionId),
	m_array			(&Static::getArray(arrayId)),
	m_kernel		(kernelId != 0 ? &Static::getKernel(kernelId) : 0),
	m_gpu			(gpuId != 0 ? &Static::getGPU(gpuId) : 0),
	m_last			(this)
{
	assert(arrayId);
	assert(m_decisionId);
	assert(m_array);
}

//-------------------------------------------------------------------
Decision::~Decision(void) {
}

//-------------------------------------------------------------------
void Decision::setFinalConfig(const variant::Hash finalHash) {
	m_finalConfig = m_array->getVariant().compress(finalHash, variant::Type::Global);
}

//-------------------------------------------------------------------
void Decision::exportDot(std::ostream& ss) {
	// add decision node
	const Array& array = *m_array;

	ss << "N" << getUID() << " [label=\"" << getId() << ":" << array.getName();
	ss << "\\n";
	
	array.getVariant().print(getFinalConfig(), ss);
	
	ss << "\", style=filled, fillcolor=green];" << std::endl;

	// traverse
	Node::exportDot(ss);
}

//-------------------------------------------------------------------
void Decision::print(void) {
	L_INFO("Decision: %s / %u", m_array->getName(), getId());
}

//-------------------------------------------------------------------
		}
	}
}