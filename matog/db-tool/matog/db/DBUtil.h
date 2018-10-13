// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DB_DBUTIL_H
#define __MATOG_DB_DBUTIL_H

#include <list>
#include <matog/shared/Degree.h>
#include <matog/log/Result.h>
#include <matog/util/SQLiteDB.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
class DBUtil {
private:
	static void clearDB					(void);
	static void clearCubins				(void);
	static void vacuum					(void);

public:
	DBUtil(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif