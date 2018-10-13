// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_GPU_H
#define __MATOG_GPU_H

#include <matog/dll.h>
#include <matog/util/String.h>
#include <matog/internal.h>
#include "DB.h"
#include <cuda.h>

namespace matog {
//----------------------------------------------------------
class MATOG_INTERNAL_DLL GPU {
private:
	uint32_t						m_id;
	const char*						m_name;
	uint32_t						m_cc;
	uint32_t						m_sharedMem;
	uint32_t						m_constMem;
	uint32_t						m_smCount;
	uint64_t						m_globalMem;
	uint32_t						m_maxThreadsPerBlock;
	uint32_t						m_maxThreadsPerSM;
	uint32_t						m_maxTexWidth;
	uint32_t						m_texAlignment;
	cuda::Architecture				m_architecture;
																	
public:
											GPU						(void);
											GPU						(const db::Stmt& stmt);
											~GPU					(void);
	inline	uint32_t						getId					(void) const { return m_id; }
	inline	const char*						getName					(void) const { return m_name; }
	inline	uint32_t						getCC					(void) const { return m_cc; }
	inline	uint64_t						getGlobalMem			(void) const { return m_globalMem; }
	inline	uint32_t						getSharedMem			(void) const { return m_sharedMem; }
	inline	uint32_t						getConstMem				(void) const { return m_constMem; }
	inline	uint32_t						getSMCount				(void) const { return m_smCount; }
	inline	uint32_t						getMaxThreadsPerBlock	(void) const { return m_maxThreadsPerBlock; }
	inline	uint32_t						getMaxThreadsPerSM		(void) const { return m_maxThreadsPerBlock; }
	inline	uint32_t						getMaxActiveBlocksPerSM	(void) const { return m_maxThreadsPerSM / m_maxThreadsPerBlock; }
	inline	uint32_t						getMaxTextureWidth		(void) const { return m_maxTexWidth; }
	inline	uint32_t						getTextureAlignment		(void) const { return m_texAlignment; }
	inline	void							invalidate				(void)		 { m_id = 0; }
	inline	bool							doesExist				(void) const { return getId() != 0; }
	inline	cuda::Architecture				getArchitecture			(void) const { return m_architecture; }

	static	GPU&				dbInsert				(std::map<uint32_t, GPU>& map, const CUdevice deviceId);
	static	void				dbSelectAll				(std::map<uint32_t, GPU>& map);
	static	void				dbDelete				(const uint32_t id);
	static	void				dbDeleteAll				(void);
};

//-------------------------------------------------------------------
}

#endif