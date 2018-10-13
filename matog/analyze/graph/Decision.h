// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_LEARN_DECISION
#define __MATOG_LEARN_DECISION

#include "Node.h"
#include <matog/analyze.h>

namespace matog {
	namespace analyze {
		namespace graph {
//-------------------------------------------------------------------
class Decision : public Node {
private:
	variant::CompressedHash	m_finalConfig;
	const uint32_t			m_decisionId;
	const Array*			m_array;
	const Kernel*			m_kernel;
	const GPU*				m_gpu;

	Node*					m_last;

public:
	Decision(const uint32_t id, const uint32_t decisionId, const uint32_t arrayId, const uint32_t kernelId, const uint32_t gpuId);
	virtual ~Decision(void);

	virtual void					exportDot		(std::ostream& ss);
	virtual void					print			(void);
			void					setFinalConfig	(const variant::Hash finalConfig);
	inline	variant::CompressedHash	getFinalConfig	(void) const  { return m_finalConfig; }
	inline	Node*					getLast			(void) const { return m_last; }
	inline	void					setLast			(Node* last) { m_last = last; }
	inline	const Array*			getArray		(void) const { return m_array; }
	inline	const Kernel*			getKernel		(void) const { return m_kernel; }
	inline	const GPU*				getGPU			(void) const { return m_gpu; }
	inline	uint32_t				getDecisionId	(void) const { return m_decisionId; }
};

//-------------------------------------------------------------------
		}
	}
}

#endif