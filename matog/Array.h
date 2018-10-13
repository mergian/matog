// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_ARRAY_H
#define __MATOG_ARRAY_H

#include <matog/dll.h>
#include <matog/util/String.h>
#include <matog/internal.h>
#include <matog/degree/Set.h>
#include <matog/variant/Type.h>
#include <matog/variant/Items.h>
#include <matog/Variant.h>
#include <cstdint>
#include <set>
#include <json/json.h>

namespace matog {
//-------------------------------------------------------------------
/// Class to represent Array Types.
/**
	This class represents Array Types inside the framework.
	It is used solely as a data container and should never be
	initialized directly. Use the class Static instead to
	access all Arrays.
*/
class MATOG_INTERNAL_DLL Array {
private:
	uint32_t		m_id;					//!< Unique id
	const char*		m_name;					//!< Name
	const char*		m_ifndef;				//!< IFNDEF-String
	uint32_t*		m_rootSizes;			//!< Root Sizes
	uint32_t		m_vdimCount;			//!< Number of variable dimensions
	uint32_t		m_rootDimCount;			//!< Number of root dimensions
	uint32_t		m_rootPtrCount;			//!< Number of ptr in root
	array::Type		m_type;					//!< Array Type
	bool			m_isStruct;				//!< Is this a AoS?
	bool			m_isCube;				//!< Is this a Cube?
	bool			m_is32BitCount;			//!< Does the implementation use a 32Bit index?
	Json::Value		m_structure;			//!< Json Object representing the structure of this Array
	const Degree*	m_rootLayout;			//!< Root layout degree
	const Degree*	m_rootMemory;			//!< Root memory degree
	const Degree*	m_rootTransposition;	//!< Root transposition degree

	variant::Items	m_items;
	Variant			m_variant;
	
	void	free (void);

protected:		
	/// Internal method used to update this Array with data from an SQL query
			void						set							(const db::Stmt& stmt);

public:
	/// Init empty Array
										Array						(void);
	/// Destructor
	virtual								~Array						(void);
	
	/// Checks if this Array is valid
	inline	bool						doesExist					(void) const { return m_id != 0; }
	
	/// Checks if two Array s are equal
	inline	bool						operator==					(const Array& other) const	{ return getId() == other.getId(); }

	/// Returns true if id of Array is lower than from other
	inline	bool						operator<					(const Array& other) const	{ return getId() < other.getId(); }

	/// Is this an AoS?
	inline	bool						isStruct					(void) const { return m_isStruct; }

	/// Is this a Cube?
	inline	bool						isCube						(void) const { return m_isCube; }

	///  Does the implementation use a 32Bit index?
	inline	bool						is32BitIndex				(void) const { return m_is32BitCount; }

	/// Returns Type of Array
	inline	array::Type					getType						(void) const { return m_type; }

	/// Return unique db id of Array
	inline	uint32_t					getId						(void) const { return m_id; }
	
	/// Return unique db id of Array
	inline	uint32_t					getRootPtrCount				(void) const { return m_rootPtrCount; }

	/// Returns Name
	inline	const char*					getName						(void) const { return m_name; }

	/// Returns IFNDEF string
	inline	const char*					getIfnDef					(void) const { return m_ifndef; }

	/// Returns the root sizes
	inline	uint32_t*					getRootSizes				(void) const { return m_rootSizes; }

	/// Invalidates this Array object
	inline	void						invalidate					(void)		 { m_id = 0; }

	/// Returns all degrees
	inline	const variant::Items&		getItems					(void) const { return m_items; }

	/// Returns variants
	inline	const Variant&				getVariant					(void) const { return m_variant; }

	/// Return vdim count
	inline	uint32_t					getVDimCount				(void) const { return m_vdimCount; }

	/// Return vdim count
	inline	uint32_t					getRootDimCount				(void) const { return m_rootDimCount; }

	/// Returns total number of available configurations
			uint32_t					getValueCount				(const variant::Type type = variant::Type::All) const;

	/// Returns the root layout degree
	inline	const Degree*				getRootLayout				(void) const { return m_rootLayout; }

	/// Returns the root memory degree
	inline	const Degree*				getRootMemory				(void) const { return m_rootMemory; }

	/// Returns the root transposition degree
	inline	const Degree*				getRootTransposition		(void) const { return m_rootTransposition; }
	
	/// Checks if the Array exists in the db
	static	bool						dbDoesExist					(const uint32_t id);

	/// Selects Array by id
	static	void						dbSelect					(Array& array, const uint32_t id);

	/// Selects Array by name
	static	void						dbSelect					(Array& array, const char* name);

	/// Selects all arrays and stores them in the map
	static	void						dbSelectAll					(std::map<uint32_t, Array>& map);

	/// Selects all degrees for all arrays in the map and updates their references
	static	void						dbSelectDegrees				(std::map<uint32_t, Array>& map);

	/// Deletes the Array from the db
	static	void						dbDelete					(const uint32_t id);

	/// Deletes the Array from the db
	static	void						dbDelete					(const char* name);

	/// Deletes all Arrays in the db
	static	void						dbDeleteAll					(void);

	/// Inserts an array into the db
	static	void						dbInsert					(array::Field& root);
};

//-------------------------------------------------------------------
}

#endif
