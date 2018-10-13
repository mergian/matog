// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/macros.h>
#include <matog/Variant.h>
#include <matog/Degree.h>
#include <matog/Array.h>
#include <matog/variant/Type.h>
#include <matog/degree/Type.h>
#include <matog/log.h>

namespace matog {
//-------------------------------------------------------------------
void Variant::init(const Items& items){
	m_items = &items;

	// clear
	m_mul.clear();
	m_mul.reserve(items.size());
	m_index.clear();
	m_print.clear();

	// reset counts
	const uint32_t typeCnt = (uint32_t)variant::Type::CNT;
	for(uint32_t i = 0; i < typeCnt; i++) {
		m_counts[i] = 1;

		if(i < (typeCnt - 1))
			m_subsets[i].clear();
	}

	// iterate items
	variant::Hash checkMax = 0;

	for(const auto& item : items) {
		// calc mul and index
		m_mul.emplace_back(count(variant::Type::All));
		m_index.emplace(MAP_EMPLACE(item, (uint32_t)m_index.size()));
		
		// calc counts
		for(uint32_t i = 0; i < typeCnt; i++) {
			const variant::Type type = (variant::Type)i;

			if(item.getDegree()->isConfigType(type)) {
				m_counts[i] *= item.getDegree()->getValueCount();

				if(type != variant::Type::All)
					m_subsets[i].emplace(item);
			}
		}

		// check if this is too complex
		THROWIF(count(variant::Type::All) < checkMax, "MATOG currently does not support more than 2^64 combinations!");
		checkMax = count(variant::Type::All);
	}

	// store print
	m_print.insert(items.begin(), items.end());
}

//-------------------------------------------------------------------
Hash Variant::count(const variant::Type type) const {
	assert(type < variant::Type::CNT);
	return m_counts[(int)type];
}

//-------------------------------------------------------------------
uint32_t Variant::index(const Degree* degree, const uint32_t paramId) const {
	return index(Item(degree, paramId));
}

//-------------------------------------------------------------------
uint32_t Variant::index(const Item& item) const {
	const auto it = m_index.find(item);
	assert(it != m_index.end());
	return it->second;
}

//-------------------------------------------------------------------
Hash Variant::mul(const Item& item) const {
	return mul(index(item));
}

//-------------------------------------------------------------------
Hash Variant::mul(const Degree* degree, const uint32_t paramId) const {
	return mul(index(degree, paramId));
}

//-------------------------------------------------------------------
Hash Variant::mul(const uint32_t index) const {
	assert(index < m_mul.size());
	return m_mul[index];
}

//-------------------------------------------------------------------
uint32_t Variant::itemCount(void) const {
	return (uint32_t)items().size();
}

//-------------------------------------------------------------------
const Items& Variant::items(void) const { 
	return *m_items;
}

//-------------------------------------------------------------------
const Set& Variant::subset(const variant::Type type) const {
	assert(type < variant::Type::All);
	return m_subsets[(int)type];
}

//-------------------------------------------------------------------
Value Variant::get(const Hash hash, const Item& item) const {
	//return get(hash, index(item));
	return (Value)((hash / mul(index(item))) % item.getDegree()->getValueCount());
}

//-------------------------------------------------------------------
Value Variant::get(const Hash hash, const Degree* degree, const uint32_t paramId) const {
	return get(hash, Item(degree, paramId));
}

//-------------------------------------------------------------------
Hash Variant::convert(const Hash inputHash, const Type outputType) const {
	Hash outputHash = 0;

	for(const Item& item : items()) {
		const Degree* d = item.getDegree();

		if(d->isConfigType(outputType)) {
			const Value value = get(inputHash, item);
			outputHash += mul(item) * value;
		}
	}

	return outputHash;
}

//-------------------------------------------------------------------
Hash Variant::hash(const uint32_t index, const Value value) const {
	return value * mul(index);
}

//-------------------------------------------------------------------
Hash Variant::hash(const Item& item, const Value value) const {
	return hash(index(item), value);
}

//-------------------------------------------------------------------
Hash Variant::hash(const Degree* degree, const uint32_t paramId, const Value value) const {
	return hash(Item(degree, paramId), value);
}

//-------------------------------------------------------------------
CompressedHash Variant::compress(const Hash input, const Type type) const {
	THROWIF(count(type) > UINT32_MAX, "unable to compress hash, as its type has more than 2^32 combinations!");

	CompressedHash newHash = 0;
	CompressedHash mul = 1;

	// first we iterate all arrays
	for(const Item& item : items()) {
		const Degree* degree = item.getDegree();

		if(degree->isConfigType(type)) {
			const Value cnt		= degree->getValueCount();
			const Value value	= get(input, item);

			newHash += value * mul;
			mul *= cnt;
		}
	}

	return newHash;
}

//-------------------------------------------------------------------
Hash Variant::uncompress(const Hash input, const variant::Type type) const {
	THROWIF(count(type) > UINT32_MAX, "unable to uncompress hash, as its type has more than 2^32 combinations!");

	Hash output = 0;
	Hash mul	= 1;

	for(const Item& item : items()) {
		const Degree* degree = item.getDegree();

		if(degree->isConfigType(type)) {	
			const Value cnt		= degree->getValueCount();
			const Value value	= (input / mul) % cnt;
			output += hash(item, value);
			mul *= cnt;
		}
	}

	return output;
}

//-------------------------------------------------------------------
void Variant::print(const Hash hash, FILE* file) const {
	const Array* last = 0;

	for(const auto& item : m_print) {
		const Degree* degree = item.getDegree();

		if(degree->getValueCount() == 1)
			continue;
		
		const Value value = get(hash, item);

		if(degree->getType() != degree::Type::Define && degree->getType() != degree::Type::Range && degree->isArray() && last != degree->getArray()) {
			last = degree->getArray();
			fprintf(file, " %s: ", last->getName());
		}

		switch(degree->getType()) {
			case degree::Type::ArrayTransposition:
			case degree::Type::Range:
				fprintf(file, "%u", value);
				break;
			case degree::Type::ArrayMemory:
			case degree::Type::ArrayLayout:
				fprintf(file, "%c", degree->printShort(value));
				break;
			case degree::Type::L1Cache:
			case degree::Type::Define:
				fprintf(file, " %s: %c ", degree->getName(), degree->printShort(value));
				break;
			default:
				fprintf(file, " %s: ??? ", degree->getName());
				break;
		}
	}
}

//-------------------------------------------------------------------
void Variant::print(const Hash hash, std::ostream& stream) const {
	const Array* last = 0;

	for(const auto& item : m_print) {
		const Degree* degree = item.getDegree();

		if(degree->getValueCount() == 1)
			continue;
		
		const Value value = get(hash, item);

		if(degree->getType() != degree::Type::Define && degree->getType() != degree::Type::Range && degree->isArray() && last != degree->getArray()) {
			last = degree->getArray();
			stream << " " << last->getName() << ": ";
		}

		switch(degree->getType()) {
			case degree::Type::ArrayTransposition:
				stream << (int)value;
				break;
			case degree::Type::ArrayMemory:
			case degree::Type::ArrayLayout:
				stream << degree->printShort(value);
				break;
			case degree::Type::L1Cache:
			case degree::Type::Define:
				stream << " " << degree->getName() << ": " << degree->printShort(value);
				break;
			case degree::Type::Range:
				stream << " " << degree->getName() << ": " << value;
				break;
			default:
				stream << " " << degree->getName() << ": ???";
				break;
		}
	}
}

//-------------------------------------------------------------------
void Variant::predictionDomainsHelper(std::list<Hash>& list, Set::const_iterator it, std::list< std::pair<Item, Value> >& pairs) const {
	// is this the end of the list?
	if(it == subset(Type::Shared).end()) {
		// no shared degrees
		Hash var = 0;

		// add shared degrees
		for(const auto& it : pairs)
			var += hash(it.first, it.second);

		list.emplace_back(var);
	} 
	// iterate all remaining items
	else {
		// get current degree
		Set::const_iterator pit = it;

		// get next iterator
		it++;

		// iterate
		for(uint32_t value = 0; value < pit->getDegree()->getValueCount(); value++) {
			pairs.emplace_back(std::make_pair(*pit, value));
			predictionDomainsHelper(list, it, pairs);
			pairs.pop_back();
		}
	}
}

//-------------------------------------------------------------------
void Variant::predictionDomains(variant::List& domains) const {
	std::list< std::pair<Item, Value> > pairs;
	predictionDomainsHelper(domains, subset(Type::Shared).begin(), pairs);
}

//-------------------------------------------------------------------
void Variant::predictionSet(variant::List& list) const {
	variant::List domains;
	predictionDomains(domains);

	for(const Hash domainHash : domains) {
		for(const Item& item : subset(Type::Independent)) {			
			const Value cnt = item.getDegree()->getValueCount();
			const Hash  mul = this->mul(index(item));

			for(Value value = 0; value < cnt; value++)
				list.emplace_back(domainHash + value * mul);
		}
	}
}

//-------------------------------------------------------------------
Hash Variant::predictionDomain(const Hash hash) const {
	Hash domain = 0;

	for(const Item& item : subset(Type::Shared)) {
		const Degree* degree	= item.getDegree();
		const Value cnt			= degree->getValueCount();
		const Hash mul			= this->mul(index(item));
		const Value value		= (Value)((hash / mul) % cnt);

		domain += value * mul;
	}

	return domain;
}

//-------------------------------------------------------------------
void Variant::predictionConfigs(const Hash hash, Hash& base, variant::List& support) const {
	// determine base
	base = predictionDomain(hash);

	// determine supports
	for(const Item& item : subset(Type::Independent)) {
		const Degree* degree	= item.getDegree();
		const Value cnt			= degree->getValueCount();
		const Hash mul			= this->mul(index(item));
		const Value value		= (Value)((hash / mul) % cnt);

		support.emplace_back(base + value * mul);
	}
}

//-------------------------------------------------------------------
}