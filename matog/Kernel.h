// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_KERNEL
#define __MATOG_KERNEL

#include <matog/dll.h>
#include <matog/DB.h>
#include <matog/internal.h>
#include <matog/variant/Items.h>
#include <matog/Variant.h>
#include <matog/enums.h>
#include <matog/kernel/hint/RWHints.h>
#include <matog/kernel/hint/CountHints.h>
#include <matog/kernel/hint/RefHints.h>

namespace matog {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL Kernel {
	uint32_t					m_id;
	uint32_t					m_moduleId;
	const char*					m_name;
	const char*					m_completeName;
	
	variant::Items				m_degrees;
	Variant						m_variant;
	kernel::hint::RWHints		m_rwHints;
	kernel::hint::CountHints	m_countHints;
	kernel::hint::RefHints		m_refHints;
		
	void	updateIndex		(void);
	void	dbSelectDegrees (void);
	void	dbSetDegrees	(void);
	void	set				(const db::Stmt& stmt);

//-------------------------------------------------------------------						
public:						
										Kernel						(void);
										~Kernel						(void);
	inline			uint32_t			getId						(void) const				{ return m_id; }
	inline	const	char*				getName						(void) const				{ return m_name; }
	inline	const	char*				getCompleteName				(void) const				{ return m_completeName; }
	inline	const 	uint32_t			getModuleId					(void) const				{ return m_moduleId; }
	inline			bool				doesExist					(void) const				{ return m_id != 0; }
	inline			bool				operator==					(const Kernel& other) const	{ return getId() == other.getId(); }
	inline			bool				operator<					(const Kernel& other) const { return getId() < other.getId(); }
	inline			void				invalidate					(void)						{ m_id = 0; }
	inline	kernel::hint::RefHints&		getRefHints					(void)						{ return m_refHints; }
	inline	const kernel::hint::RefHints&	getRefHints				(void) const				{ return m_refHints; }

	static			void				dbSelectAll					(std::map<uint32_t, Kernel>& map);
					void				dbSelect					(const uint32_t id);
					void				dbSelect					(const char* name);
	static			uint32_t			dbInsert					(const char* name, const uint32_t moduleId);
					void				dbSelectOrInsert			(const char* name, const uint32_t moduleId);
	static			void				dbDelete					(const uint32_t moduleId);
	static			void				dbDeleteAll					(void);
	static			uint32_t			dbSelectId					(const char* name);

					void				setDegrees					(const variant::Set& set);
			const	variant::Items&		getDegrees					(void) const { return m_degrees; }
			const	char*				generateMatogCode			(variant::Hash hash, const cuda::Architecture architecture) const;
					bool				areDegreesInitialized		(void) const;
	inline	const	Variant&			getVariant					(void) const { return m_variant; }

	inline			RWMode				getRWMode(const uint32_t arrayId, uint32_t paramId) const	{ return m_rwHints.getMode(arrayId, paramId); }
};

//-------------------------------------------------------------------
}

#endif