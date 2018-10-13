// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_VARIANT
#define __MATOG_VARIANT

#include <matog/dll.h>
#include <matog/internal.h>
#include <matog/variant/Type.h>
#include <matog/variant/Mul.h>
#include <matog/variant/Index.h>
#include <matog/variant/Set.h>
#include <matog/variant/Print.h>
#include <matog/variant/Items.h>
#include <list>

namespace matog {
//-------------------------------------------------------------------
using namespace matog::variant;

class MATOG_INTERNAL_DLL Variant {
private:
	const Items*	m_items;									//<! items
	Hash			m_counts	[(int)Type::CNT];				//<! number of elements for each hash type
	Index			m_index;									//<! index for config
	Mul				m_mul;										//<! magic number for hash
	Set				m_subsets	[(int)Type::CNT - 1];			//<! subset items
	Print			m_print;									//<! print

	/// calculates the hash for a single value
	Hash hash(const uint32_t index, const Value value) const;

	/// helper function
	void predictionDomainsHelper(std::list<Hash>& list, Set::const_iterator it, std::list< std::pair<Item, Value> >& pairs) const;

public:
	void			init			(const Items& items);									//!< inits the object
	Hash			count			(const Type type = Type::All) const;					//!< gets the count for a specific type
	uint32_t		index			(const Item& item) const;								//!< gets the index
	uint32_t		index			(const Degree* degree, const uint32_t paramId) const;	//!< gets the index
	Hash			mul				(const Item& item) const;								//!< gets the magic number
	Hash			mul				(const Degree* degree, const uint32_t paramId) const;	//!< gets the magic number
	Hash			mul				(const uint32_t index) const;							//!< gets the magic number
	uint32_t		itemCount		(void) const;											//!< gets the number of config items
	const Items&	items			(void) const;											//!< gets the items

	const Set&		subset			(const Type type) const;								//!< gets a set of items

	//Value			get				(const Hash hash, const uint32_t index) const;			//!< fetches a value of a hash for a given index
	Value			get				(const Hash hash, const Item& item) const;
	Value			get				(const Hash hash, const Degree* degree, const uint32_t paramId) const;

	template<typename T> T get		(const Hash hash, const Item& item) const { return (T)get(hash, item); }
	template<typename T> T get		(const Hash hash, const Degree* degree, const uint32_t paramId) const { return (T)get(hash, degree, paramId); }

	Hash			convert			(const Hash input, const Type outputType) const;
	Hash			hash			(const Item& item, const Value value) const;
	Hash			hash			(const Degree* degree, const uint32_t paramId, const Value value) const;
	CompressedHash	compress		(const Hash input, const Type type) const;
	Hash			uncompress		(const Hash input, const Type type) const;
	void			print			(const Hash hash, FILE* file = stdout) const;
	void			print			(const Hash hash, std::ostream& stream) const;

	void			predictionSet		(variant::List& list) const;
	void			predictionConfigs	(const Hash hash, Hash& base, variant::List& support) const;
	void			predictionDomains	(variant::List& domains) const;
	Hash			predictionDomain	(const Hash hash) const;
};

//-------------------------------------------------------------------
}

#endif