// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeDefine.h"
#include <matog/log.h>
#include <matog/Degree.h>
#include <matog/util/String.h>

using namespace matog::util;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CodeDefine::CodeDefine(Json::Value root) {
	Json::Value _name = root.get("name", Json::Value());
	THROWIF(!_name.isString(), "DEFINE::NAME", "has to be a string value");
	String name = _name.asString();
	name.trim();
	THROWIF(name.isEmpty(), "DEFINE::NAME", "cannot be empty string");

	Json::Value _isShared = root.get("is_shared", false);
	THROWIF(!_isShared.isBool(), "DEFINE::IS_SHARED", "has to be boolean value");
	const bool isShared = _isShared.asBool();

	std::list< std::pair<String, uint32_t> > options;
	
	Json::Value _options = root.get("options", Json::Value());
	THROWIF(!_options.isArray(), "DEFINE::OPTIONS", "has to be an array");

	for(Json::Value node : _options) {
		THROWIF(!node.isObject(), "DEFINE::OPTIONS", "has to be an object");
		THROWIF(node.size() != 1, "DEFINE::OPTIONS", "only one member per option allowed");

		String name = node.getMemberNames().front();
		name.trim();
		THROWIF(name.isEmpty(), "DEFINE::OPTIONS::NAME", "cannot be empty string");

		uint32_t value = UINT_MAX;
		for(Json::Value member : node) {
			THROWIF(!member.isInt(), "DEFINE::OPTIONS::VALUE", "has to be integer value");
			value = member.asInt();
		}

		options.emplace_back(std::make_pair(name, value));
	}

	THROWIF(options.empty(), "DEFINE", "no options found!");

	Degree::dbInsertDefine(name, options, isShared);
}

//-------------------------------------------------------------------
	}
}