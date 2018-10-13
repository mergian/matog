// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/Module.h>
#include <matog/Array.h>
#include <matog/Static.h>
#include <matog/Kernel.h>
#include <matog/db/PredictionModel.h>
#include <matog/kernel/Meta.h>
#include <matog/db/tables.h>
#include <matog/db/Stmt.h>
#include <matog/cuda/CUBINStore.h>
#include <matog/util/IO.h>
#include <matog/util/String.h>
#include <matog/log.h>
#include <fstream>
#include <matog/macros.h>

using matog::db::Stmt;
using matog::util::IO;
using matog::util::String;

namespace matog {
//-------------------------------------------------------------------
// STATIC
//-------------------------------------------------------------------
void Module::dbSelect(const uint32_t id) {
	invalidate();

	Stmt stmt("SELECT id, name, source, modified FROM " DB_MODULES " WHERE id = ?;");
	stmt.bind(0, id);
	
	THROWIF(!stmt.step());
	set(stmt);
}

//-------------------------------------------------------------------
void Module::dbSelectAll(std::map<uint32_t, Module>& map) {
	Stmt stmt("SELECT id, name, source, modified FROM " DB_MODULES ";");
	
	while(stmt.step())
		map.emplace(MAP_EMPLACE(stmt.get<uint32_t>(0), stmt));
}

//-------------------------------------------------------------------
void Module::dbSelect(const char* name) {
	invalidate();

	Stmt stmt("SELECT id, name, source, modified FROM " DB_MODULES " WHERE name = ?;");
	stmt.bind(0, name);
	
	THROWIF(!stmt.step());
	set(stmt);
}

//-------------------------------------------------------------------
uint32_t Module::dbInsert(const char* name, const char* source) {
	Stmt stmt("INSERT INTO " DB_MODULES " (name, source, modified) VALUES (?, ?, 0);");
	stmt.bind(0, name);
	stmt.bind(1, source);
	return stmt.step32();
}

//-------------------------------------------------------------------
uint32_t Module::dbSelectId(const char* name) {
	Stmt stmt("SELECT id FROM " DB_MODULES " WHERE name = ?;");
	stmt.bind(0, name);

	if(stmt.step())
		return stmt.get<uint32_t>(0);

	return UINT_MAX;
}

//-------------------------------------------------------------------
bool Module::addArray(const Array* a) {
	// return immediately if this array already exists
	if(!m_arrays.emplace(a).second)
		return false;

	for(const auto& it : a->getItems())
		addDegree(it.getDegree());

	return true;
}

//-------------------------------------------------------------------
bool Module::addDegree(const Degree* d) {
	// return immediately if this degree already exists
	if(!m_degrees.emplace(d).second)
		return false;

	return true;
}

//-------------------------------------------------------------------
void Module::updateFromSource(void) {
	// check if the file has changed
	if(!hasChanged())
		return;
	
	// delete CUBINS and MODULE DB ENTRIES and the Models, so that a new profiling has to be performed
	cuda::CUBINStore	::dbDelete(getId());
	Kernel				::dbDelete(getId());
	kernel::Meta		::dbDeleteByModule(getId());
	Module				::dbDeleteDeps(getId());
	db::PredictionModel	::dbDeleteAll();
	// TODO: we cannot clear the profiling here, as it removes data from the current profilign...
	// Profiling		::dbDeleteAll();
	
	// warn user
	L_DEBUG("parsing '%s'", getSourceFile());

	// clear all local data
	m_degrees.clear();
	m_arrays.clear();

	for(auto& d : Static::getFunctionDegrees()) {
		m_degrees.emplace(d);

	// parse file
		{}
		FileSet alreadyRead;
		readFile(getSourceFile(), alreadyRead);
	}

	// degrees
	Stmt stmt("INSERT INTO " DB_MODULE_DEGREES " (module_id, degree_id) VALUES (?, ?);");
	stmt.bind(0, getId());

	for(auto& d : m_degrees) {
		if(d->isCompiler()) {
			stmt.reset();
			stmt.bind(1, d->getId());
			stmt.step();
		}
	}

	// get modified time + update module
	L_TRACE("updating module");
	m_modified	= IO::getModificationTime(getSourceFile());

	Stmt("UPDATE " DB_MODULES " SET modified = ? WHERE id = ?;")
		.bind(0, m_modified)
		.bind(1, getId())
		.step();
}

//-------------------------------------------------------------------
// INSTANCE
//-------------------------------------------------------------------
Module::Module(void) :
	m_id			(0),
	m_name			(0),
	m_source		(0),
	m_modified		(0)
{}

//-------------------------------------------------------------------
Module::Module(const Stmt& stmt) :
	m_id			(0),
	m_name			(0),
	m_source		(0),
	m_modified		(0)
{
	set(stmt);
}

//-------------------------------------------------------------------
void Module::set(const Stmt& stmt) {
	THROWIF(m_id != 0);
	THROWIF(m_modified != 0);
	THROWIF(m_name != 0);
	THROWIF(m_source != 0);
	
	m_id			= stmt.get<uint32_t>(0);
	m_modified		= stmt.get<uint64_t>(3);
	m_name			= NEW_STRING(stmt.get<const char*>(1));
	m_source		= NEW_STRING(stmt.get<const char*>(2));
	
	dbSelectDeps();
}

//-------------------------------------------------------------------
Module::~Module(void) {
	if(m_name)
		FREE_A(m_name);

	if(m_source)
		FREE_A(m_source);
}

//-------------------------------------------------------------------
void Module::dbSelectDeps(void) {
	Stmt stmt("SELECT degree_id FROM " DB_MODULE_DEGREES " WHERE module_id = ?;");
	stmt.bind(0, getId());

	while(stmt.step()) {
		const Degree& d = Static::getDegree(stmt.get<uint32_t>(0));

		if(d.isArray())
			addArray(d.getArray());
		else
			addDegree(&d);
	}

	// init function degrees
	for(const Degree* d : Static::getFunctionDegrees())
		addDegree(d);
}

//-------------------------------------------------------------------
void Module::readFile(const String file, FileSet& alreadyRead) {
	// other files
	FileList list;
	const String path = IO::getFilePath(file);

	// process file
	if(IO::doesFileExist(file)) {
		// open file
		std::ifstream in(file.c_str());

		// iterate all lines 
		String line;
		
		while(in.good()) {
			// get new line
			std::string tmp;
			std::getline(in, tmp);

			// trim line
			line = tmp;
			line.trim();

			// only process preprocessor directives
			if(line.startsWith("#")) {
				// add new includes
				if(line.startsWith("#include")) {
					const String file = IO::simplifyPath(extractInclude(line, path));

					if(file.isEmpty()) {
						L_WARN("Unable to parse #include statement in file '%s'. Possible coding error.", file.c_str());
					} else {
						list.push_back(file);
					}
				}
				else {
					// check for Defines
					for(const Degree* d : Static::getDefines()) {
						if(line.find(d->getName()) != UINT_MAX) {
							if(addDegree(d))
								L_DEBUG("using define: %s", d->getName());
						}
					}

					// check for Arrays
					std::list<String> list;
					line.explode(list, ' ');

					// skip if the list is empty
					if(list.empty())
						continue;

					std::list<String>::const_iterator listItem = list.begin();

					// check
					if(*listItem == "#define") {
						const String param = line.substr((uint32_t)strlen("#define")).trim();

						// iterate all vars and check if the names fit the defines
						for(auto& item : Static::getArrays()) {
							if(param == item.second.getIfnDef()) {
								if(addArray(&item.second))
									L_DEBUG("using array: %s", item.second.getName());
								break;
							} 
						}
					}
				}
			}
		}

		// iterate all files
		for(auto& item : list) {
			// check if we already had that file
			if(alreadyRead.find(item) == alreadyRead.end()) {
				alreadyRead.insert(item);
				readFile(item, alreadyRead);
			}
		}

		// close
		in.close();
	}
}

//-------------------------------------------------------------------
String Module::extractInclude(const String& line, const String& path) {
	uint32_t pos = line.find('<');
	const bool relative = pos == UINT_MAX;
	
	String filename;

	if(relative) {
		filename = path;
		pos = line.find('"');
	}

	String tmp = line.substr(pos + 1, line.getLength() - 1);

	if(tmp.isEmpty())
		return String();

	filename.append(tmp);

	return filename;
}

//-------------------------------------------------------------------
bool Module::hasChanged(void) const {
	if(getModificationTime() == 0)
		return true;
	
	if(IO::getModificationTime(getSourceFile()) > getModificationTime())
		L_WARN("kernel file has changed modification time!");
	
	return false;
}

//-------------------------------------------------------------------
void Module::dbDeleteAll(void) {
	Stmt("DELETE FROM " DB_MODULES ";").step();
	Stmt("DELETE FROM sqlite_sequence WHERE NAME = '" DB_MODULES "';").step();
}

//-------------------------------------------------------------------
void Module::dbDeleteAllDeps(void) {
	Stmt("DELETE FROM " DB_MODULE_DEGREES ";").step();
}

//-------------------------------------------------------------------
void Module::dbDeleteDeps(const uint32_t moduleId) {
	Stmt("DELETE FROM " DB_MODULE_DEGREES " WHERE module_id = ?;").bind(0, moduleId).step();
}

//-------------------------------------------------------------------
}