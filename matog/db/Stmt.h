// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DB_STMT
#define __MATOG_DB_STMT

#include <matog/dll.h>
#include <matog/util/SQLStmt.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
struct MATOG_INTERNAL_DLL Stmt : public util::SQLStmt {
	Stmt();
	Stmt(const char* query);
};

//-------------------------------------------------------------------
	}
}

#endif