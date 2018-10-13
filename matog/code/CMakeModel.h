// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_CMAKEMODEL_H
#define __MATOG_CODE_CMAKEMODEL_H

#include <list>
#include <map>
#include <string>
#include <utility>
#include <matog/templates/Singleton.h>
#include "Template.h"

namespace matog { 
	namespace code {
//-------------------------------------------------------------------
class CMakeModel : public templates::Singleton<CMakeModel>, public Template {
private:
	static CMakeModel* s_instance;

public:
	// typedefs
	struct File {
		std::string name;
		bool isHeader;
	};

	typedef std::list<File> SourceGroupList_t;
	typedef std::map<std::string, SourceGroupList_t> SourceGroupMap_t;

private:
	// variables
	SourceGroupMap_t			m_sourceFiles;
	std::string					m_libName;
	
	// constructor
	CMakeModel(void);
	void check(void) const;

public:
	// methods
			void			generate			(void);
	
	// setter
	void setMinVersion			(const std::string& minVersion);
	void addDefinition			(const std::string& definition);
	void addIncludeDirectory	(const std::string& includeDirectory);
	void addCudaIncludeDirectory(const std::string& includeDirectory);
	void addLinkDirectory		(const std::string& linkDirectory);
	void addPreInclude			(const std::string& include);
	void addPostInclude			(const std::string& include);
	void addSet					(const std::string& key, const std::string& value);
	void addSourceFile			(const std::string& sourceFile, const std::string& sourceGroup, const bool isHeader);
	void addSourceFiles			(const SourceGroupList_t& sourceFiles, const std::string& sourceGroup);
	void addLibrary				(const std::string& lib);
	void addFindPath			(const std::string& findPath);
	void addFindModule			(const std::string& moduleName, const bool required, const std::string& version, const std::list<std::string>& components);
	void addSubDirectory		(const std::string& subDirectory);
	void setLibName				(const std::string& libName);
	const std::string& getLibName(void) const;

	// friends
	friend class templates::Singleton<CMakeModel>;
};

//-------------------------------------------------------------------
	}
}

#endif