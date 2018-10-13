// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/db/Stmt.h>
#include <matog/DB.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
Stmt::Stmt() : util::SQLStmt(DB::getInstance()) {}

//-------------------------------------------------------------------
Stmt::Stmt(const char* query) : util::SQLStmt(query, DB::getInstance()) {}

//-------------------------------------------------------------------
	}
}