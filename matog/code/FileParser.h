// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_FILEPARSER_H
#define __MATOG_CODE_FILEPARSER_H

#include <matog/templates/Singleton.h>
#include <matog/util/String.h>
#include <json/json.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class FileParser : public templates::Singleton<FileParser> {
private:
	static FileParser* s_instance;

	// classes
	class SourceGroup {
	private:
		// variables
		util::String	m_hostHeader;
		util::String	m_hostSource;
		util::String	m_deviceSource;

	public:
		// constructor
		SourceGroup(void);
		SourceGroup(const util::String& hostHeader, const util::String& hostSource, const util::String& deviceSource);

		// methods
		bool operator<(const SourceGroup& otherGroup) const;

		// getter
		const util::String& getHostHeader(void) const;
		const util::String& getHostSource(void) const;
		const util::String& getDeviceSource(void) const;
	};
	
	// constructor
				FileParser			(void);

public:
	void parse(void);

	// friends
	friend class templates::Singleton<FileParser>;
};

//-------------------------------------------------------------------
	}
}

#endif