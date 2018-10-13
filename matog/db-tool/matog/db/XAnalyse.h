// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DB_XANALYSE_H
#define __MATOG_DB_XANALYSE_H

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
class XAnalyse {
private:
	typedef std::unordered_map<uint64_t, uint64_t> DurationMap;
	typedef std::unordered_map<uint32_t, DurationMap> CallMap;


	shared::DB&					m_db;
	const char*					m_gpuName;
	const char*					m_benchmarkName;
	std::vector<const char*>	m_algorithms;
	CallMap						m_durations;


	log::Result loadAllDurations(void);
	log::Result analyse(const char* algorithm);
	log::Result attach(const char* algorithm);
	log::Result detach(void);


public:
	XAnalyse(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif