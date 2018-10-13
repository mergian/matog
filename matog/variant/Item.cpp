// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/Degree.h>
#include <matog/degree/Set.h>
#include <matog/variant/Item.h>

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
bool Item::operator<(const Item& other) const {
	const int result = degree::SetComparator::compare(getDegree(), other.getDegree());

	if(result == 1)
		return true;

	if(result == -1)
		return false;

	return m_paramId < other.m_paramId;
}

//-------------------------------------------------------------------
bool Item::operator==(const Item& other) const {
	return m_degree->getId() == other.m_degree->getId() && m_paramId == other.m_paramId;
}

//-------------------------------------------------------------------
Item& Item::operator=(const Item& other) {
	m_degree  = other.getDegree();
	m_paramId = other.getParamId();
	return *this;
}

//-------------------------------------------------------------------
	}
}