// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CMakeModel.h"
#include <matog/log.h>
#include <sstream>
#include <fstream>

using ctemplate::TemplateDictionary;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CMakeModel* CMakeModel::s_instance = 0;

//-------------------------------------------------------------------
CMakeModel::CMakeModel(void) : Template("CMake") {
	// set min version
	setMinVersion("2.8");

	// add required modules
	addFindModule("CUDA",  true, "", std::list<std::string>());
	addFindModule("MATOG", true, "", std::list<std::string>());
	addFindPath("$ENV{MATOG_DIR}");

	// add required libraries
	addLibrary("${CUDA_CUDA_LIBRARY}");
	addLibrary("${MATOG_LIBRARIES}");

	// add include dirs
	addIncludeDirectory("${MATOG_INCLUDE_DIR}");
	addIncludeDirectory("${CUDA_TOOLKIT_ROOT_DIR}/extras/CUPTI/include");
	addIncludeDirectory("${CMAKE_CURRENT_LIST_DIR}/..");
	addIncludeDirectory("${CMAKE_CURRENT_LIST_DIR}");

	// no min max for the lib!
	addDefinition("-DNOMINMAX");
}

//-------------------------------------------------------------------
void CMakeModel::check(void) const {
	// is executable name set?
	THROWIF(getLibName().empty(), "CODE_MATOG_XML_NO_LIBNAME_FOUND");
}

//-------------------------------------------------------------------
void CMakeModel::setMinVersion(const std::string& minVersion) {
	if(!minVersion.empty())
		getDict()->SetValue("MIN_VERSION", minVersion.c_str());
}

//-------------------------------------------------------------------
void CMakeModel::addDefinition(const std::string& definition) {
	if(!definition.empty())
		(*getDict()->AddSectionDictionary("DEFINITIONS"))["DEFINITION"] = definition.c_str();
}

//-------------------------------------------------------------------
void CMakeModel::addIncludeDirectory(const std::string& includeDirectory) {
	if(!includeDirectory.empty())
		(*getDict()->AddSectionDictionary("INCLUDE_DIRECTORIES"))["INCLUDE_DIRECTORY"] = includeDirectory.c_str();
}

//-------------------------------------------------------------------
void CMakeModel::addCudaIncludeDirectory(const std::string& includeDirectory) {
	if(!includeDirectory.empty())
		(*getDict()->AddSectionDictionary("CUDA_INCLUDE_DIRECTORIES"))["CUDA_INCLUDE_DIRECTORY"] = includeDirectory.c_str();
}

//-------------------------------------------------------------------
void CMakeModel::addLinkDirectory(const std::string& linkDirectory) {
	if(!linkDirectory.empty())
		(*getDict()->AddSectionDictionary("LINK_DIRECTORIES"))["LINK_DIRECTORY"] = linkDirectory.c_str();
}

//-------------------------------------------------------------------
void CMakeModel::addPreInclude(const std::string& include) {
	if(!include.empty())
		(*getDict()->AddSectionDictionary("PRE_INCLUDES"))["PRE_INCLUDE"] = include.c_str();
}

//-------------------------------------------------------------------
void CMakeModel::addPostInclude(const std::string& include) {
	if(!include.empty())
		(*getDict()->AddSectionDictionary("POST_INCLUDES"))["POST_INCLUDE"] = include.c_str();
}

//-------------------------------------------------------------------
void CMakeModel::addSet(const std::string& key, const std::string& value) {
	if(!key.empty() && !value.empty()) {
		TemplateDictionary& dict = *getDict()->AddSectionDictionary("SETS");
		dict["SET_KEY"]		= key.c_str();
		dict["SET_VALUE"]	= value.c_str();
	}
}

//-------------------------------------------------------------------
void CMakeModel::addSourceFile(const std::string& sourceFile, const std::string& sourceGroup, const bool isHeader) {
	if(!sourceFile.empty()) {
		SourceGroupList_t list;
		File f;
		f.name		= sourceFile;
		f.isHeader	= isHeader;
		list.push_back(f);
		addSourceFiles(list, sourceGroup);
	}
}

//-------------------------------------------------------------------
void CMakeModel::addSourceFiles(const SourceGroupList_t& sourceFiles, const std::string& sourceGroup) {
	if(!sourceFiles.empty()) {
		SourceGroupMap_t::iterator it = m_sourceFiles.find(sourceGroup);

		// allocate new if list does not exist.
		if(it == m_sourceFiles.end()) {
			m_sourceFiles[sourceGroup] = SourceGroupList_t();
			it = m_sourceFiles.find(sourceGroup);
		}

		it->second.insert(it->second.end(), sourceFiles.begin(), sourceFiles.end());
	}
}

//-------------------------------------------------------------------
void CMakeModel::addLibrary(const std::string& lib) {
	if(!lib.empty())
		(*getDict()->AddSectionDictionary("LINK_LIBRARIES"))["LINK_LIBRARY"] = lib.c_str();
}

//-------------------------------------------------------------------
void CMakeModel::addFindPath(const std::string& findPath) {
	if(!findPath.empty())
		(*getDict()->AddSectionDictionary("FIND_PATHS"))["FIND_PATH"] = findPath.c_str();
}

//-------------------------------------------------------------------
void CMakeModel::addFindModule(const std::string& moduleName, const bool required, const std::string& version, const std::list<std::string>& components) {
	if(!moduleName.empty()) {
		TemplateDictionary& dict = *getDict()->AddSectionDictionary("FIND_MODULES");
		dict["MODULE_NAME"]		= moduleName.c_str();
		dict["MODULE_REQUIRED"]	= required ? "REQUIRED" : "";
		dict["MODULE_VERSION"]	= version.c_str();

		std::ostringstream ss;

		if(!components.empty()) {
			ss << "COMPONENTS";

			for(auto& item : components) {
				ss << " " << item;
			}
		}

		dict["MODULE_COMPONENTS"]	= ss.str().c_str();
	}
}

//-------------------------------------------------------------------
void CMakeModel::addSubDirectory(const std::string& subDirectory) {
	if(!subDirectory.empty())
		(*getDict()->AddSectionDictionary("SUB_DIRECTORIES"))["SUB_DIRECTORY"] = subDirectory.c_str();
}

//-------------------------------------------------------------------
void CMakeModel::generate(void) {
	// perform checks
	check();

	L_INFO("Generating CMake project...");

	// set lib name
	getDict()->SetValue("LIB_NAME", getLibName().c_str());

	// iterate all source groups
	SourceGroupList_t sourceFiles;

	if(!m_sourceFiles.empty()) {
		for(auto& item : m_sourceFiles) {
			// get source group
			const std::string sourceGroup = item.first;

			// get files
			SourceGroupList_t list = item.second;

			// add files to target list
			sourceFiles.insert(sourceFiles.end(), list.begin(), list.end());

			// skip if this source group is the root
			if(sourceGroup.empty())
				continue;
		
			// escape strings for CMAKE
			std::ostringstream ss;
		
			size_t start	= 0;
			size_t end		= 0;

			while((end = sourceGroup.find("\\", start)) != std::string::npos) {
				ss << sourceGroup.substr(start, end - start);
			
				// add only double dashs if we really need them!
				if((end + 1) < sourceGroup.size()) {
					if(sourceGroup.c_str()[end + 1] != '\\')
						ss << "\\\\";
				}

				start = end + 1;
			}

			ss << sourceGroup.substr(start);

			// write command
			TemplateDictionary& dict = *getDict()->AddSectionDictionary("SOURCE_GROUPS");
			dict["SOURCE_GROUP"] = ss.str().c_str();

			// iterate all files
			std::ostringstream fs;
			for(auto& group : list) {
				fs << "\t" << group.name << std::endl;
			}

			dict["SOURCE_FILES"] = fs.str().c_str();
		}

		// declare header files, so they won't be compiled
		std::ostringstream hs;

		for(auto& file : sourceFiles) 
			if(file.isHeader)
				hs << "\t" << file.name << std::endl;
	
		getDict()->SetValue("HEADER_FILES", hs.str().c_str());
	}	

	{ // add source files
		std::ostringstream fs;
		for(auto& file : sourceFiles)
			fs << "\t" << file.name << std::endl;

		getDict()->SetValue("SOURCE_FILES", fs.str().c_str());
	}
	
	// generate filename
	{
		// open file
		std::ofstream file("matog/CMakeLists.txt");

		// can we access the file?
		THROWIF(!file, "IO_EXCEPTION");

		file << expandTemplate("CMakeLists");

		// we are done!
		THROWIF(!file.good(), "IO_EXCEPTION");

		file.close();
	}
}

//-------------------------------------------------------------------
const std::string& CMakeModel::getLibName(void) const {
	return m_libName;
}

//-------------------------------------------------------------------
void CMakeModel::setLibName(const std::string& libName) {
	m_libName = libName;
}

//-------------------------------------------------------------------
	}
}