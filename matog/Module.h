// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_MODULE_H
#define __MATOG_MODULE_H

#include <matog/dll.h>
#include <matog/internal.h>
#include <matog/templates/Singleton.h>
#include <matog/degree/Set.h>
#include <matog/array/Set.h>
#include <matog/util.h>
#include <map>

namespace matog {
//-------------------------------------------------------------------
#ifdef WIN32
template class MATOG_INTERNAL_DLL std::map<const Degree*, uint32_t>;
#endif

//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL Module {
private:
			typedef std::set<util::String>								FileSet;
			typedef std::list<util::String>								FileList;

			uint32_t				m_id;
			const char*				m_name;
			const char*				m_source;
			uint64_t				m_modified;

			degree::Set				m_degrees;
			array::Set				m_arrays;
	
			void					set						(const db::Stmt& stmt);
			
			void					readFile				(const util::String file, FileSet& alreadyRead);
	static	util::String			extractInclude			(const util::String& line, const util::String& path);

			bool					addDegree				(const Degree* d);
			bool					addArray				(const Array* a);
			void					dbSelectDeps			(void);

//-------------------------------------------------------------------
public:
									Module					(void);
									Module					(const db::Stmt& stmt);
									~Module					(void);
	inline	const	char*			getName					(void) const	{ return m_name; }
	inline	const	char*			getSourceFile			(void) const	{ return m_source; }
	inline			uint32_t		getId					(void) const	{ return m_id; }
	inline			uint64_t		getModificationTime		(void) const	{ return m_modified; }
	inline			bool			doesExist				(void) const	{ return m_id != 0; }
	inline	const	degree::Set&	getDegrees				(void) const	{ return m_degrees; }
	inline	const	array::Set&		getArrays				(void) const	{ return m_arrays; }
	inline	uint32_t				getDegreeCount			(void) const	{ return (uint32_t)m_degrees.size(); }
	inline	uint32_t				getArrayCount			(void) const	{ return (uint32_t)m_arrays.size(); }
			bool					hasChanged				(void) const;
	inline	void					invalidate				(void) { m_id = 0; }
	inline	bool					operator==				(const Module& other) const { return getId() == other.getId(); }
	inline	bool					operator<				(const Module& other) const { return getId() < other.getId(); }

			void					dbSelect				(const uint32_t id);
			void					dbSelect				(const char* name);
	static	void					dbSelectAll				(std::map<uint32_t, Module>& map);
	static	uint32_t				dbInsert				(const char* name, const char* source);
	static	void					dbDeleteAll				(void);
	static	void					dbDeleteDeps			(const uint32_t moduleId);
	static	void					dbDeleteAllDeps			(void);
	static	uint32_t				dbSelectId				(const char* name);

			void					updateFromSource		(void);
};
//-------------------------------------------------------------------
}

#endif