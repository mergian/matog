// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYSE_GRAPH_CALL
#define __MATOG_ANALYSE_GRAPH_CALL

#include "Node.h"
#include <matog/analyze.h>
#include <matog/enums.h>
#include <vector>

namespace matog {
	namespace analyze {
		namespace graph {
//-------------------------------------------------------------------
class Call : public Node {
private:
	const Kernel*				m_kernel;
	const Module*				m_module;
	const GPU*					m_gpu;
	call::ItemMap				m_items;
	variant::CompressedHash		m_finalHash;
	call::ParamIds				m_paramIds;
	uint32_t					m_decisionId;

	//---------------------------------------------------------------
	void	init			(void);
	void	loadTimes		(void);

public:
			Call	(const uint32_t id, const uint32_t kernelId, const uint32_t gpuId, const uint32_t decisionId);
			Call	(const Call&) = delete;
	virtual ~Call	(void);

	inline const Module*					getModule			(void) const { return m_module; }
	inline const Kernel*					getKernel			(void) const { return m_kernel; }
	inline const GPU*						getGPU				(void) const { return m_gpu; }
			void							setFinalConfig		(const variant::Hash finalHash);
	inline	variant::CompressedHash			getFinalConfig		(void) const  { return m_finalHash; }
			call::Item						getItem				(const variant::Hash hash) const;
	inline const call::ParamIds&			getParamIds			(void) const { return m_paramIds; }
	virtual void							exportDot			(std::ostream& ss);
	virtual void							print				(void);
	inline	uint32_t						getDecisionId		(void) const { return m_decisionId; }
};

//-------------------------------------------------------------------
		}
	}
}

#endif