// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Header.h"
#include <matog/log.h>
#include <matog/util/IO.h>

using namespace matog::util;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
Header::Header(void) : m_file(""), m_isLocal(false) {
}

//-------------------------------------------------------------------
Header::Header(const String& file, const bool isLocal) : m_isLocal(isLocal) {
	m_path = IO::getFilePath(file);
	m_file = IO::getFileName(file);
}

//-------------------------------------------------------------------
bool Header::operator==(const Header& header) const {
	if(!(m_file == header.m_file))
		return false;
	
	if(!(m_path == header.m_path))
		return false;

	return m_isLocal == header.m_isLocal;
}

//-------------------------------------------------------------------
bool Header::isEmpty(void) const {
	return m_file.isEmpty();
}

//-------------------------------------------------------------------
String Header::getHeader(const String relativePath) const {
	std::ostringstream ss;
	ss << (m_isLocal ? "\"" : "<");
	
	if(m_isLocal && !relativePath.isEmpty()) {
		const std::list<String> headerPath = IO::pathToList(m_path);
		const std::list<String> relPath	= IO::pathToList(relativePath);
		
		std::list<String>::const_iterator hit = headerPath.begin();
		std::list<String>::const_iterator rit = relPath.begin();

		// find matching folders
		while(hit != headerPath.end() && rit != relPath.end()) {
			// break if the folders do not match
			if(!((*hit) == (*rit)))
				break;

			hit++;
			rit++;
		}

		// add ../
		for(; rit != relPath.end(); rit++) {
			ss << "../";
		}

		// add required folders
		for(; hit != headerPath.end(); hit++) {
			ss << *hit << "/";
		}

		ss << m_file;
	} else {
		ss << m_path << m_file;
	}

	ss << (m_isLocal ? "\"" : ">");
	return ss.str();
}

//-------------------------------------------------------------------
	}
}