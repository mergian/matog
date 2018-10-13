// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DB_XBESTVARIANT_H
#define __MATOG_DB_XBESTVARIANT_H

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
class XBestVariant {
private:
	typedef std::map<uint8_t, uint32_t> CountMap;
	typedef std::map<MatogConfig::Type, CountMap> TypeMap;
	typedef std::map<uint32_t, TypeMap> KernelMap;

	shared::DB&					m_db;
	const char*					m_gpuName;
	const char*					m_benchmarkName;
	const char*					m_file;
	std::vector<const char*>	m_algorithms;
	KernelMap					m_map;

	log::Result analyse(void);

public:
	XBestVariant(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif