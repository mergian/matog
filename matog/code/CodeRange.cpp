// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeRange.h"
#include <matog/log.h>
#include <matog/Degree.h>
#include <matog/util/String.h>

using namespace matog::util;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CodeRange::CodeRange(Json::Value root) {
	Json::Value _name = root.get("name", Json::Value());
	THROWIF(!_name.isString(), "RANGE::NAME", "has to be a string value");
	String name = _name.asString();
	name.trim();
	THROWIF(name.isEmpty(), "RANGE::NAME", "cannot be empty string");

	Json::Value _isShared = root.get("is_shared", false);
	THROWIF(!_isShared.isBool(), "RANGE::IS_SHARED", "has to be boolean value");
	const bool isShared = _isShared.asBool();

	Json::Value _max = root.get("max", 0.0);
	THROWIF(!_max.isDouble(), "RANGE::MAX", "has to be a double value");
	
	Json::Value _min = root.get("min", 0.0);
	THROWIF(!_min.isDouble(), "RANGE::MIN", "has to be a double value");
	
	Json::Value _step = root.get("step", 0.0);
	THROWIF(!_step.isDouble(), "RANGE::STEP", "has to be a double value");

	const double min	= _min.asDouble();
	const double max	= _max.asDouble();
	const double step	= _step.asDouble();

	THROWIF(min >= max, "min has to be bigger than max");
	THROWIF(step <= 0, "step has to be > 0");

	Degree::dbInsertRange(name, min, max, step, isShared);
}

//-------------------------------------------------------------------
	}
}