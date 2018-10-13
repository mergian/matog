// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/Variant.h>
#include <matog/Degree.h>
#include <matog/variant/Iterator.h>
#include <cassert>

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
Iterator::Iterator(Set::iterator end) {
	m_state.first	= end;
	m_state.second	= 0;
}

//-------------------------------------------------------------------
Iterator::Iterator(const Variant& variant, const Items& fixedItems) :
	m_fixedHash(0)
{
	// init free items
	m_freeItems.insert(variant.items().begin(), variant.items().end());
	
	// init fixed hash and remove from freeItems
	for(const auto& pair : fixedItems) {
		m_fixedHash += variant.hash(pair.first, pair.second);
		m_freeItems.erase(pair.first);
	}

	// init state
	m_state.first  = m_freeItems.begin();
	m_state.second = 0;
}

//-------------------------------------------------------------------
Iterator& Iterator::operator++(void) {
	// get cnt
	const Value cnt = m_state.first->getDegree()->getValueCount();
	
	// increment value
	m_state.second++;
	assert(m_state.second <= cnt);

	// increment item, if the value has reached cnt
	if(m_state.second == cnt) {
		m_state.first++;
		m_state.second = 0;
	}

	return *this;
}

//-------------------------------------------------------------------
Iterator Iterator::end(void) {
	return Iterator(m_freeItems.end());
}

//-------------------------------------------------------------------
bool Iterator::operator==(const Iterator& other) const {
	return m_state.first == other.m_state.first && m_state.second == other.m_state.second;
}

//-------------------------------------------------------------------
bool Iterator::operator!=(const Iterator& other) const {
	return !(*this == other);
}

//-------------------------------------------------------------------
	}
}