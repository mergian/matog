// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeVar.h"
#include "CMakeModel.h"
#include <matog/log.h>
#include <matog/util.h>
#include <matog/array/field/Type.h>
#include "Host.h"
#include "Shared.h"
#include "Global.h"
#include "Dyn.h"
#include <vector>

#define min(A, B) (A < B ? A : B)

using namespace matog::util;
using namespace matog::array;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CodeVar::CodeVar(Json::Value root) {
	// parse
	parse(root, m_root);

	m_root.initNames();
	m_root.sort();
	
	// add variable and solutions to DB
	Array::dbInsert(m_root);
}

//-------------------------------------------------------------------
CodeVar::~CodeVar(void) {
}

//-------------------------------------------------------------------
void CodeVar::parse(Json::Value& jRoot, Field& field) {
	// GENERIC STUFF ------------------------------------------------
	// get name
	Json::Value jName = jRoot.get("name", Json::Value::nullSingleton());
	THROWIF(&field == &m_root && !jName.isString(), "name has to be string value");

	// get type
	Json::Value jType = jRoot.get("type", "struct");
	THROWIF(!jType.isString(), "type has to be string value");

	// init field
	field(jName.isString() ? jName.asCString() : "FIELD", field::getTypeByName(jType.asCString()));

	// STRUCT -------------------------------------------------------
	// is this a struct?
	if(field.getType() == field::Type::STRUCT) {
		// get fields
		Json::Value jFields = jRoot.get("fields", Json::Value::nullSingleton());
		THROWIF(!jFields.isArray(), "fields have to be an array");

		for(Json::Value jNode : jFields) {
			Field& tmp  = field.addChild();
			parse(jNode, tmp);
		}
	}

	// MULTI-DIM ----------------------------------------------------
	// get counts
	Json::Value jCounts = jRoot.get("counts", Json::Value::nullSingleton());
	if(jCounts.isArray()) {
		for(Json::Value jNode : jCounts) {
			THROWIF(!jNode.isUInt(), "counts have to be unsigned int!");
			const uint32_t value = jNode.asUInt();

			THROWIF(&field != &m_root && value == 0, "only the count of the root array can be 0");
			field.addCount(value);
		}
	} else if(&field == &m_root) {
		THROW("The root node requires at least one count!");
	}

	// internal index type
	Json::Value jCube = jRoot.get("is_cube", false);
	THROWIF(!jCube.isBool(), "is_cube has to be a bool");
	field.setCube(jCube.asBool());

	THROWIF(field.isCube() && field.getVDimCount() != 1, "all counts have to be 0, if the is_cube=true");
	THROWIF(field.isCube() && !field.isRoot(), "only the root array can be a cube!");

	// internal index type
	Json::Value jInternalIndexType = jRoot.get("internal_index_type", 32);
	THROWIF(!jInternalIndexType.isInt(), "internal_index_type has to be an integer");
	THROWIF(jInternalIndexType.asInt() != 32 && jInternalIndexType.asInt() != 64, "internal_index_type has to be 32 or 64");

	// external index type
	Json::Value jExternalIndexType = jRoot.get("external_index_type", 32);
	THROWIF(!jExternalIndexType.isInt(), "external_index_type has to be an integer");
	THROWIF(jExternalIndexType.asInt() != 32 && jExternalIndexType.asInt() != 64, "external_index_type has to be 32 or 64");
		
	// set internal/external index type
	field.setIndexType(jInternalIndexType.asInt() == 32 ? field::Type::U32 : field::Type::U64, jExternalIndexType.asInt() == 32 ? field::Type::U32 : field::Type::U64);

	// parse index + count only if necessary
	if(field.getDimCount() > 1) {
		// custom count
		Json::Value jCount = jRoot.get("count", Json::Value::nullSingleton());
		if(jCount.isString())
			field.setCustomCount(jCount.asCString());
		
		// custom index
		Json::Value jIndex = jRoot.get("index", Json::Value::nullSingleton());
		
		// auto index
		if(jIndex.isString()) {
			std::string s(jIndex.asString());
			std::list<std::string> placeholders;
			std::list<std::string> variables;

			// generate placeholder string, and write placeholder into vector
			for(uint32_t i = 0; i < m_root.getDimCount(); i++) {
				const char c = (char)('A' + i);

				if(s.find(c) != std::string::npos) {
					placeholders.emplace_back("p");
					placeholders.back().append({c});

					variables.emplace_back();
					variables.back().append({c});

					const std::string& what = variables.back();
					const std::string& with = placeholders.back();

					// replace all
					for(size_t start_pos = s.find(what); start_pos != std::string::npos; start_pos = s.find(what, start_pos + with.length()))
						s.replace(start_pos, what.length(), with);
				}
			}

			// generate transpositions
			std::list<std::string> sList;
			sList.emplace_back(s);
			for(const std::string& v : variables) {
				std::list<std::string> csList;
				for(const std::string& s : sList) {
					for(const std::string& p : placeholders) {
						if(s.find(p) != std::string::npos) {
							std::string cs(s);

							const std::string& what = p;
							const std::string& with = v;

							// replace all
							for(size_t start_pos = cs.find(what); start_pos != std::string::npos; start_pos = cs.find(what, start_pos + with.length()))
								cs.replace(start_pos, what.length(), with);

							csList.emplace_back(cs);
						}
					}
				}
				sList = csList;
			}

			for(const std::string& index : sList)
				field.addIndex(index.c_str());
		} 
		// auto index
		else if(jIndex.isArray()) {
			for(Json::Value item : jIndex) {
				THROWIF(!item.isString(), "index has to be a string value");
				field.addIndex(item.asCString());
			}
		} 
		// default index
		else {
			THROWIF(field.isCustomCount(), "cannot use default indexing with a custom count!");
			
			// recursive function
			std::function<void (std::list<uint32_t>, std::vector<uint32_t>, const uint32_t)> func = [&](std::list<uint32_t> result, std::vector<uint32_t> items, const uint32_t useId) {
				if(!items.empty()) {
					// add item to list
					result.push_back(items[useId]);

					// remove item from list
					items.erase(items.begin() + useId);

					// are items empty?
					if(!items.empty()) {
						// iterate all remaining items
						for(uint8_t i = 0; i < items.size(); i++) {
							func(result, items, i);
						}
					} else {
						std::ostringstream ss, sa;

						bool isFirst = true;

						for(auto& item : result) {
							if(!isFirst) {
								ss << " + " << sa.str() << " * ";
								sa << " * ";
							} else {
								isFirst = false;
							}

							ss << (char)('A' + item);
							sa << "_" << (char)('A' + item);
						}

						field.addIndex(ss.str().c_str());
					}
				}
			};

			// init data structures
			std::vector<uint32_t> items;
			items.resize(field.getDimCount());

			for(uint32_t i = 0; i < field.getDimCount(); i++)
				items[i] = i;

			std::list<uint32_t> result;

			// iterate
			for(uint32_t i = 0; i < field.getDimCount(); i++)
				func(result, items, i);
		}
	}
}


//-------------------------------------------------------------------
void CodeVar::generate(void) const {
	Host h(m_root);
	Global g(m_root);
	Shared s(m_root);
	Dyn d(m_root);
}

//-------------------------------------------------------------------
CodeVar::operator String(void) const {
	return m_root.getName();
}

//-------------------------------------------------------------------
	}
}