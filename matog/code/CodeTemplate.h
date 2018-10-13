// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_CODETEMPLATE
#define __MATOG_CODE_CODETEMPLATE

#include "Template.h"
#include "CodeHelper.h"
#include <fstream>
#include <matog/internal.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class CodeTemplate : public Template, protected CodeHelper {
protected:
	const array::Field& m_root;

	void					initCounts			(const array::Field& child, const char* label, const bool isRoot64Bit, const bool usesMembers = true, const bool usesHint = false) const;
	void					initElementCounts	(const array::Field& child, const char* label, const bool isRoot64Bit) const;

public:
	CodeTemplate			(const array::Field& root, const bool isRoot64Bit);
	virtual ~CodeTemplate	(void);
	
	void generateFile(const char* tmpl, const char* nameExtension, const char* fileExtension) const;
	void openFile(::std::ofstream& stream, const char* nameExtension, const char* fileExtension) const;
	void closeFile(::std::ofstream& stream) const;
	
};

//-------------------------------------------------------------------
	}
}

#endif