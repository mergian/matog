// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_HEADER_H
#define __MATOG_CODE_HEADER_H

#include <matog/util/String.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class Header {
private:
	util::String	m_file;
	util::String	m_path;
	bool		m_isLocal;

public:
	Header(void);
	Header(const util::String& file, const bool isLocal);
	
			util::String getHeader	(const util::String relPath = "") const;

			bool		isEmpty		(void) const;
			bool		operator==	(const Header& header) const;

	static const Header CUDA;
	static const Header MATOG;
	static const Header MATOGVAR;
	static const Header MATOGMALLOC;
	static const Header EMPTY;
	static const Header VECTOR_TYPES;
};

//-------------------------------------------------------------------
	}
}
#endif