// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DB_XWATCHDOGOVERHEAD_H
#define __MATOG_DB_XWATCHDOGOVERHEAD_H

#include <vector>
#include <map>
#include <matog/shared/Kernel.h>
#include <matog/shared/Module.h>
#include <matog/shared/Degree.h>
#include <matog/shared/DB.h>
#include <matog/log/Result.h>
#include <matog/util/SQLiteDB.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
class XWatchDogOverhead {
private:
	shared::DB&					m_db;
	const char*					m_file;
	const char*					m_gpu;
	const char*					m_benchmark;
	
	log::Result analyse(void);

public:
	XWatchDogOverhead(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif