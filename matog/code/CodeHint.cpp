// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeHint.h"
#include <matog/log.h>
#include <matog/Static.h>
#include <matog/enums.h>
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CodeHint::CodeHint(Json::Value root, const char* kernelName) {
	THROWIF(!root.isObject(), "member of hints has to be an object");

	// for each array
	const std::vector<std::string> names = root.getMemberNames();

	for(const std::string name : names) {
		// get array
		const size_t pos = name.find('/');
		THROWIF(pos == std::string::npos, "array has to be defined by ARRAYNAME/PARAMID");
		const std::string arrayName = name.substr(0, pos);
		const uint32_t paramId = std::atoi(name.c_str() + pos + 1);
		const Array& array = Static::getArray(arrayName.c_str());
		
		// get node
		Json::Value node = root.get(name, Json::Value::nullSingleton());
		THROWIF(node.isNull());

		// get mode
		Json::Value _mode = node.get("mode", Json::Value::nullSingleton());
		RWMode mode = RWMode::READ_AND_WRITE;

		if(_mode.isString()) {
			const std::string str = _mode.asString();
			const bool isRead  = str.find('r') != std::string::npos;
			const bool isWrite = str.find('w') != std::string::npos;
			const bool isOnce  = str.find('o') != std::string::npos;

			if(!isWrite) {
				mode = RWMode::READ_ONLY;
			} else {
				if(!isRead)
					mode = RWMode::WRITE_ONLY;
				else if(isOnce)
					mode = RWMode::READ_ONCE;
			}
		}

		db::Stmt modeStmt("INSERT INTO " DB_KERNEL_HINTS_RW " (kernel_name, array_id, param_id, mode) VALUES (?, ?, ?, ?);");
		modeStmt.bind(0, kernelName);
		modeStmt.bind(1, array.getId());
		modeStmt.bind(2, paramId);
		modeStmt.bind(3, mode);
		modeStmt.step();

		// get counts
		Json::Value _counts = node.get("counts", Json::Value::nullSingleton());

		db::Stmt refStmt("INSERT INTO " DB_KERNEL_HINTS_REF " (kernel_name, array_id, param_id, dim, src_array_id, src_param_id, src_dim) VALUES (?, ?, ?, ?, ?, ?, ?);");
		refStmt.bind(0, kernelName);
		refStmt.bind(1, array.getId());
		refStmt.bind(2, paramId);

		db::Stmt cntStmt("INSERT INTO " DB_KERNEL_HINTS_COUNTS " (kernel_name, array_id, param_id, dim, value) VALUES (?, ?, ?, ?, ?);");
		cntStmt.bind(0, kernelName);
		cntStmt.bind(1, array.getId());
		cntStmt.bind(2, paramId);

		if(_counts.isArray()) {
			uint32_t sourceDim = 0;

			for(Json::Value value : _counts) {
				// is numeric value?
				if(value.isInt64()) {
					cntStmt.bind(3, sourceDim);
					cntStmt.bind(4, value.asInt64());
					cntStmt.step();
					cntStmt.reset();
				}
				// is reference value?
				else if(value.isString()) {
					const std::string valueStr = value.asString();
					const size_t pos1 = valueStr.find('/');
					const size_t pos2 = valueStr.find('/', pos1+1);
					THROWIF(pos1 == std::string::npos || pos2 == std::string::npos, "array has to be defined by ARRAYNAME/PARAMID/DIM");
					const std::string refName = valueStr.substr(0, pos1);
					const Array& refArray = Static::getArray(refName.c_str());
					const uint32_t refParamId = std::atoi(valueStr.substr(pos1 + 1,pos2).c_str());
					const uint32_t refDim = std::atoi(valueStr.c_str() + pos2 + 1);
					
					refStmt.bind(3, sourceDim);
					refStmt.bind(4, refArray.getId());
					refStmt.bind(5, refParamId);
					refStmt.bind(6, refDim);
					refStmt.step();
					refStmt.reset();
				}

				// inc dim
				sourceDim++;
			}
		}
	}
}

//-------------------------------------------------------------------
	}
}