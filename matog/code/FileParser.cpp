// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "FileParser.h"
#include "CMakeModel.h"
#include "CodeModel.h"
#include "CodeRange.h"
#include "CodeHint.h"
#include "CodeVar.h"
#include "CodeDefine.h"
#include "FileParser.h"
#include <matog/log.h>
#include <matog/util/IO.h>
#include <matog/DB.h>
#include <matog/Static.h>

using namespace matog::util;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
FileParser* FileParser::s_instance = 0;

//-------------------------------------------------------------------
class SrcFilter : public IO::FileFilter {
public:
	virtual bool isValid(const String& name) const {
		return name.endsWith(".cu") || name.endsWith(".h") || name.endsWith(".cpp") || name == "CMakeLists.txt"; 
	}
};
	
//-------------------------------------------------------------------
FileParser::FileParser(void) {
}

//-------------------------------------------------------------------
void FileParser::parse(void) {
	const char* jsonFile = "matog-code.json";

	// does file exist?
	THROWIF(!IO::doesFileExist(jsonFile), "MATOG_JSON_NOT_FOUND");

	// open JSON document
	Json::Reader reader;
	Json::Value root;
	std::ifstream file(jsonFile);
	THROWIF(!file, "UNABLE_TO_OPEN_MATOG_JSON");

	try {
		reader.parse(file, root, false);	
		const std::string errors = reader.getFormattedErrorMessages();

		if(!errors.empty()) {
			L_ERROR("JSON parsing errors:\n%s", errors.c_str());
			exit(1);
		}
	} catch(const std::runtime_error& e) {
		THROW("JSON_ERROR", e.what());
	}
	
	// get lib name
	Json::Value _libname = root.get("libname", Json::Value());
	THROWIF(_libname.isNull(), "MISSING_LIBNAME")
	const std::string libname = _libname.asString();

	// create folder
	IO::mkdir("matog");
	
	// setup db
	DB::setFailIfDoesNotExist(false);
	DB::setSetupNewDB(true);
	
	// set libname
	CMakeModel::getInstance().setLibName(libname);

	// Arrays
	Json::Value arrays = root.get("arrays", Json::Value());
	if(arrays.isArray()) {
		for(auto it = arrays.begin(); it != arrays.end(); it++)
			CodeModel::getInstance().addVariable(NEW(CodeVar(*it)));
	}

	// Defines
	Json::Value defines = root.get("defines", Json::Value());
	if(defines.isArray()) {
		for(auto it = defines.begin(); it != defines.end(); it++)
			CodeDefine define(*it);
	}

	// Ranges
	Json::Value ranges = root.get("ranges", Json::Value());
	if(ranges.isArray()) {
		for(auto it = ranges.begin(); it != ranges.end(); it++)
			CodeRange range(*it);
	}
	
	Static::init();

	// Hints
	Json::Value hints = root.get("hints", Json::Value());
	if(hints.isObject()) {
		const std::vector<std::string> names = hints.getMemberNames();
		for(const std::string& name : names)
			CodeHint hint(hints.get(name, Json::Value::nullSingleton()), name.c_str());
	}
}

//-------------------------------------------------------------------
FileParser::SourceGroup::SourceGroup(void) :
	m_hostHeader("Header Files"),
	m_hostSource("Source Files"),
	m_deviceSource("Source Files\\device")
{}

//-------------------------------------------------------------------
FileParser::SourceGroup::SourceGroup(const String& hostHeader, const String& hostSource, const String& deviceSource) : 
	m_hostHeader(hostHeader),
	m_hostSource(hostSource),
	m_deviceSource(deviceSource)
{}

//-------------------------------------------------------------------
bool FileParser::SourceGroup::operator<(const SourceGroup& otherGroup) const {
	if(m_hostHeader < otherGroup.m_hostHeader)
		return true;

	if(m_hostSource < otherGroup.m_hostSource)
		return true;

	if(m_deviceSource < otherGroup.m_deviceSource)
		return true;

	return false;
}

//-------------------------------------------------------------------
const String& FileParser::SourceGroup::getHostHeader(void) const {
	return m_hostHeader;
}

//-------------------------------------------------------------------
const String& FileParser::SourceGroup::getHostSource(void) const {
	return m_hostSource;
}

//-------------------------------------------------------------------
const String& FileParser::SourceGroup::getDeviceSource(void) const {
	return m_deviceSource;
}

//-------------------------------------------------------------------
	}
}