// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_KERNEL_META
#define __MATOG_KERNEL_META

#include <matog/internal.h>

namespace matog {
	namespace kernel {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL Meta {
//-------------------------------------------------------------------
private:
	uint32_t		m_staticSharedMem;
	uint32_t		m_constMem;
	uint32_t		m_localMem;
	uint32_t		m_usedRegisters;

	void operator()(const db::Stmt& stmt);

//-------------------------------------------------------------------						
public:						
										Meta				(void);
	inline	uint32_t					getStaticSharedMem	(void) const	{ return m_staticSharedMem; }
	inline	uint32_t					getConstMem			(void) const	{ return m_constMem; }
	inline	uint32_t					getLocalMem			(void) const	{ return m_localMem; }
	inline	uint32_t					getUsedRegisters	(void) const	{ return m_usedRegisters; }
	inline	bool						doesExist			(void) const	{ return m_usedRegisters > 0; }
			void						invalidate			(void);
	
	static	void						dbSelect			(Meta& meta, const variant::Hash moduleHash, const uint32_t userHash, const uint32_t cc, const uint32_t Kernelid);
	static	void						dbInsert			(const variant::Hash moduleHash, const uint32_t userHash, const uint32_t cc, const uint32_t kernelId, const uint32_t staticSharedMem, const uint32_t constMem, const uint32_t localMem, const uint32_t usedRegisters);
	static	void						dbDeleteByKernel	(const uint32_t kernelId);
	static	void						dbDeleteByModule	(const uint32_t moduleId);
	static	void						dbDeleteByCUBIN		(const uint32_t cubinHash);
	static	void						dbDeleteAll			(void);
};

//-------------------------------------------------------------------
	}
}

#endif