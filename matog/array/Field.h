// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ARRAY_FIELD
#define __MATOG_ARRAY_FIELD

#include <matog/internal.h>
#include <matog/util/String.h>
#include <matog/array/field/VType.h>
#include <matog/array/field/Memory.h>
#include <vector>
#include <set>

namespace matog {
	namespace array {
//-------------------------------------------------------------------
/// Field descriptor for AoS Array Types
class MATOG_INTERNAL_DLL Field {
	const char*				m_name;
	const char*				m_globalName;
	const char*				m_sharedName;
	const char*				m_dynName;
	field::Type				m_type;
	const Field*			m_parent;

	std::list<Field>		m_sub;
	std::vector<uint32_t>	m_counts;

	uint32_t				m_globalId;
	uint32_t				m_globalLayoutId;
	uint32_t				m_globalTranspositionId;
	uint32_t				m_globalMemoryId;

	uint32_t				m_sharedId;
	uint32_t				m_sharedLayoutId;
	uint32_t				m_sharedTranspositionId;    
	
	uint32_t				m_dynId;
	uint32_t				m_dynLayoutId;
	uint32_t				m_dynTranspositionId;

	field::Type				m_internalIndexType;
	field::Type				m_externalIndexType;

	bool					m_isCube;

	const char*				m_customCount;
	std::list<const char*>	m_indexes;
		
public:
	Field			(const Field* parent = 0);
	~Field			(void);

	void operator()(const char* name, const field::Type type);

	// array setter methods
	Field&		addChild				(void);
	void		addCount				(const uint32_t count);
	void		initNames				(void);
	void		sort					(void);

	void		setGlobalId				(const uint32_t id);
	void		setGlobalLayoutId		(const uint32_t id);
	void		setGlobalTranspositionId(const uint32_t id);
	void		setGlobalMemoryId		(const uint32_t id);

	void		setSharedId				(const uint32_t id);
	void		setSharedLayoutId		(const uint32_t id);
	void		setSharedTranspositionId(const uint32_t id);

	void		setDynId				(const uint32_t id);
	void		setDynLayoutId			(const uint32_t id);
	void		setDynTranspositionId	(const uint32_t id);
	void		setIndexType			(const field::Type internalIndexType, const field::Type externalIndexType);
	void		setCube					(const bool value);

	// getter
	const std::list<Field>& getChildren		(void) const;
	std::list<Field>&		getChildren		(void);
	const std::vector<uint32_t>& getCounts	(void) const;
	const char*		getName					(void) const;
	const char*		getGlobalName			(void) const;
	const char*		getSharedName			(void) const;
	const char*		getDynName				(void) const;
	uint32_t		getSize					(void) const;
	uint32_t		getTreeSize				(void) const;
	field::Type		getType					(void) const;
	field::Type		getInternalIndexType	(void) const;
	field::Type		getExternalIndexType	(void) const;
	const char*		getTypeName				(void) const;
	const char*		getVTypeName			(void) const;
	const Field*	getParent				(void) const;
	bool			isVector				(void) const;
	bool			isConstant				(void) const;

	uint32_t		getDimCount				(void) const;
	uint32_t		getVDimCount			(void) const;
	uint32_t		getTranspositionCount	(void) const;
	
	bool		isFloat					(void) const;
	bool		isUnsigned				(void) const;
	bool		isPrimitive				(void) const;
	bool		isCube					(void) const;
	bool		isMultiDimensional		(void) const;
	bool		isAOSOA					(void) const;
	bool		isCustomCount			(void) const;
	bool		isSubArrays				(void) const;
	
	bool		isRoot					(void) const;
	bool		isStruct				(void) const;
	bool		isLayoutStruct			(void) const;
	bool		isDimensional			(void) const;
	bool		hasChildren				(void) const;

	uint32_t	getGlobalId				(void) const;
	uint32_t	getGlobalLayoutId		(void) const;
	uint32_t	getGlobalTranspositionId(void) const;
	uint32_t	getGlobalMemoryId		(void) const;

	uint32_t	getSharedId				(void) const;
	uint32_t	getSharedLayoutId		(void) const;
	uint32_t	getSharedTranspositionId(void) const;

	uint32_t	getDynId				(void) const;
	uint32_t	getDynLayoutId			(void) const;
	uint32_t	getDynTranspositionId	(void) const;

	uint32_t	getElementCount			(void) const;

	const char*						getCustomCount	(void) const;
	void							setCustomCount	(const char* equation);
	const std::list<const char*>&	getIndex		(void) const;
	void							addIndex		(const char* index);

	bool		operator<				(const Field& field) const;
};

//-------------------------------------------------------------------
	}
}

#endif