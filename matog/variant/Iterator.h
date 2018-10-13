// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_VARIANT_ITERATOR
#define __MATOG_VARIANT_ITERATOR

#include <matog/internal.h>
#include <matog/variant/Item.h>
#include <matog/variant/Set.h>
#include <vector>

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
/// Iterator class to iterate hashes, 
class MATOG_INTERNAL_DLL Iterator {
public:
	typedef std::vector< std::pair<Item, Value> > Items;

private:
	Set								m_freeItems;
	std::pair<Set::iterator, Value>	m_state;
	Hash							m_fixedHash;

	Iterator(Set::iterator end);

public:
	Iterator(const Variant& variant, const Items& fixedItems);
	Iterator& operator++(void);
	operator Hash(void);
		
	Iterator end(void);
	bool operator==(const Iterator& other) const;
	bool operator!=(const Iterator& other) const;
};

//-------------------------------------------------------------------
	}
}

#endif