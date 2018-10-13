// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/Options.h>
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/util/Env.h>
#include <matog/log.h>
#include <fstream>
#include <thread>
#include <sstream>
#include <string>
#include <list>

using matog::util::Env;

//-------------------------------------------------------------------
void printHelper(Json::Value value, const uint32_t depth = 0) {
	// generate spacing
	std::ostringstream ss;
	for(uint32_t i = 0; i < depth; i++) {
		ss << "-";
	}
	ss << "> ";
	
	const std::string str = ss.str();

	// recurse if is object
	if(value.isObject()) {
		for(const std::string& name : value.getMemberNames()) {
			Json::Value child = value.get(name, Json::Value::nullSingleton());

			if(child.isObject()) {
				L_DEBUG_
				printf("%s", str.c_str());
				
				matog::log::setColor(matog::log::Color::GREEN);
				printf("%s", name.c_str());
				matog::log::resetColor();
				printf(":");

				L_END
				printHelper(child, depth + 1);
			} else {
				// print value
				L_DEBUG_
				printf("%s", str.c_str());
				
				matog::log::setColor(matog::log::Color::TEAL);
				printf("%s", name.c_str());
				matog::log::resetColor();

				printf(": ");

				if(child.isArray()) {
					printf("[");
					for(auto it = child.begin(); it != child.end(); it++) {
						if(it != child.begin())
							printf(", ");
			
						printf("%s", it->asCString());
					}
					printf("]");
				}
				else if(child.isBool())		printf("%s [bool]",		child.asBool() ? "true" : "false");
				else if(child.isUInt())		printf("%u [uint]",		child.asUInt());
				else if(child.isUInt64())	printf("%llu [uint64]",	(unsigned long long int)child.asUInt64());
				else if(child.isInt())		printf("%i [int]",		child.asInt());
				else if(child.isInt64())	printf("%lli [int64]",	(long long int)child.asInt());		
				else if(child.isDouble())	printf("%f [double]",	child.asDouble());
				else if(child.isString())	printf("%s [string]",	child.asCString());
				else						printf("???");
				L_END;
			}
		}
	}
}

//-------------------------------------------------------------------

namespace matog {
//-------------------------------------------------------------------
Options* Options::s_instance = 0;

//-------------------------------------------------------------------
template<> MATOG_INTERNAL_DLL bool Options::getValue<bool>(const std::initializer_list<const char*>& key, const bool defaultValue) const { 
	Json::Value value = getNode(key); 
	if(value.empty()) return defaultValue;
	return value.asBool(); 
}

//-------------------------------------------------------------------
template<> MATOG_INTERNAL_DLL Json::Value Options::getValue<Json::Value>(const std::initializer_list<const char*>& key, const Json::Value defaultValue) const { 
	Json::Value value = getNode(key); 
	if(value.empty()) return defaultValue;
	return value;
}

//-------------------------------------------------------------------
#define GETVALUE(TYPE, CHECK, CONVERT) \
template<> MATOG_INTERNAL_DLL TYPE Options::getValue<TYPE>(const std::initializer_list<const char*>& key, const TYPE defaultValue) const { \
	Json::Value value = getNode(key); \
	if(value.empty()) return defaultValue; \
	errorif(!value.CHECK(), key); \
	return value.CONVERT(); \
}

//-------------------------------------------------------------------
GETVALUE(int32_t, isInt64, asInt)
GETVALUE(int64_t, isInt64, asInt64)
GETVALUE(uint32_t, isUInt64, asUInt)
GETVALUE(uint64_t, isUInt64, asUInt64)
GETVALUE(float, isDouble, asFloat)
GETVALUE(double, isDouble, asDouble)

//-------------------------------------------------------------------
Json::Value Options::getNode(const std::initializer_list<const char*>& key) const {
	const Json::Value null;
	Json::Value node = m_root;

	for(const char* i : key) {
		if(node.isObject())
			node = node.get(i, null);
		else
			return null;
	}

	return node;
}

//-------------------------------------------------------------------
void Options::errorif(const bool isError, const std::initializer_list<const char*>& key) {
	if(isError) {
		std::ostringstream ss;

		for(auto it = key.begin(); it != key.end(); it++) {
			if(it != key.begin())
				ss << "::";

			ss << *it;
		}

		const std::string msg = ss.str();
		THROW("Invalid value for config entry: %s", msg .c_str());
	}
}

//-------------------------------------------------------------------
void Options::init(void) {
	getInstance();
}

//-------------------------------------------------------------------
Options::Options(void) {
	loadJson();
}

//-------------------------------------------------------------------
const char* Options::getDefaultFileName(void) {
	return "matog.json";
}

//-------------------------------------------------------------------
void Options::loadJson(void) {
	Json::Reader reader;

	const char* fileName = Env::get("MATOG_CONFIG");
	if(!fileName)
		fileName = getDefaultFileName();

	std::ifstream file(fileName);
	if(file) {
		try {
			reader.parse(file, m_root, false);	
		} catch(const std::runtime_error& e) {
			THROW("JSON_ERROR", e.what());
		}
		
		file.close();
	}

	// init log options
	log::setLevel			(getValue<log::Level>({"log", "level"},		log::getLevel(), &log::getLevelFromString));
	log::setPrintFilename	(getValue<bool>({"log", "print_filename"},	log::isPrintFilename()));
	log::setPrintLevel		(getValue<bool>({"log", "print_level"},		log::isPrintLevel()));
	log::setPrintTime		(getValue<bool>({"log", "print_time"},		log::isPrintTime()));
	log::setUseColors		(getValue<bool>({"log", "color"},			log::isUseColors()));

	// print
	if(log::isDebug()) {
		L_DEBUG("using config file: %s", fileName);
		print();
	}
}

//-------------------------------------------------------------------
void Options::dbInsert(const uint32_t profilingId) {
	// init builder
	Json::StreamWriterBuilder builder;
	builder.settings_["commentStyle"] = "None";
	builder.settings_["indentation"] = "\t";

	// init writer
	Json::StreamWriter* writer = builder.newStreamWriter();
	
	// write
	std::ostringstream ss;
	writer->write(Options::getInstance().m_root, &ss);

	std::string str;
	str = ss.str();

	// free writer
	delete writer;
	
	// export to db
	db::Stmt stmt("INSERT INTO " DB_CONFIG " (profiling_id, json) VALUES (?, ?);");
	stmt.bind(0, profilingId);
	stmt.bind(1, str.c_str());
	stmt.step();
}

//-------------------------------------------------------------------
void Options::dbDelete(const uint32_t profilingId) {
	db::Stmt stmt("DELETE FROM " DB_CONFIG " WHERE profiling_id = ?;");
	stmt.bind(0, profilingId);
	stmt.step();
}

//-------------------------------------------------------------------
void Options::dbDeleteAll(void) {
	db::Stmt("DELETE FROM " DB_CONFIG ";").step();
}

//-------------------------------------------------------------------
void Options::print(void) const {
	L_DEBUG("-- MATOG Config begin --");
	printHelper(m_root);
	L_DEBUG("-- MATOG Config end --");
}

//-------------------------------------------------------------------
}