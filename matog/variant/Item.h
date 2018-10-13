// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_VARIANT_ITEM
#define __MATOG_VARIANT_ITEM

#include <matog/dll.h>
#include <cstdint>
#include <matog/internal.h>

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
/// An item represents a Degree and its ParamId and provides comparators
class MATOG_INTERNAL_DLL Item {
	const Degree*	m_degree;	//!< Degree
	uint32_t		m_paramId;	//!< ParamId

public:
	/// Empty Constructor
	inline Item(void) : m_degree(0), m_paramId(0) {}

	/// Copy Constructor
	inline Item(const Item& other) : m_degree(other.getDegree()), m_paramId(other.getParamId()) {}

	/// Constructor
	inline Item(const Degree* degree, const uint32_t paramId) : m_degree(degree), m_paramId(paramId) {}

	/// Comparator for map and set
	bool operator<(const Item& other) const;

	/// Equal Comparator
	bool operator==(const Item& other) const;

	/// = operator
	Item& operator=(const Item& other);

	/// Returns the degree
	const Degree*	getDegree	(void) const { return m_degree; }

	/// Returns the ParamId
	uint32_t		getParamId	(void) const { return m_paramId; }
};

//-------------------------------------------------------------------
	}
}

#endif