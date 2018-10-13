// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/shared.h>
#include <matog/log.h>
#include <matog/util.h>
#include <matog/cuda.h>
#include "../db.h"
#include <cfloat>
#include <cmath>

#ifdef WIN32
#pragma warning(disable:4996)
#endif

namespace matog {
	namespace db {
//-------------------------------------------------------------------
XWatchDogOverhead::XWatchDogOverhead(int argc, char** argv) :
	m_db			(DB::getInstance()),
	m_file			(0),
	m_gpu			(0),
	m_benchmark		(0)
{
	if(argc < 4) {
		L_ERROR("usage: SECONDDB GPU BENCHMARK FILE");
		exit(1);
	}

	m_gpu		= argv[1];
	m_benchmark	= argv[2];
	m_file		= argv[3];
	
	CHECK(Static::init());
	m_db.attach(argv[0], "tmp");

	CHECK(analyse());
}

//-------------------------------------------------------------------
Result XWatchDogOverhead::analyse(void) {
	struct Item {
		uint64_t wdDuration;
		uint64_t hash;
		uint32_t callId;
		uint64_t duration;
		uint32_t kernelId;

		Item(const uint64_t _wdDuration, const uint64_t _hash, const uint32_t _callId, const uint32_t _kernelId) : wdDuration(_wdDuration), hash(_hash), callId(_callId), duration(0), kernelId(_kernelId)
		{}
	};
	
	// init items
	L_INFO("loading wd items...");
	std::list<Item> items;

	sqlite3_stmt* stmt;
	RCHECK(m_db.prepare(stmt, "SELECT duration, hash, call_id, kernel_id FROM tmp.runs, tmp.calls WHERE tmp.runs.duration != 0 AND tmp.runs.call_id = tmp.calls.id AND tmp.calls.kernel_id "
		"IN (SELECT kernels.id FROM kernels, modules WHERE kernels.module_id = modules.id AND modules.uses_wd = 1);"));
	
	SQL_LOOP(stmt)
		items.emplace_back(sqlite3_column_int64(stmt, 0), sqlite3_column_int64(stmt, 1), sqlite3_column_int(stmt, 2), sqlite3_column_int(stmt, 3));
	SQL_LEND()
	RCHECK(m_db.finalize(stmt));

	// update duration from main table
	L_INFO("loading items...");
	struct Item2 {
		double min;
		double max;
		double avg;
		double var;
		double abw;
		uint32_t cnt;

		Item2(void) : min(DBL_MAX), max(-DBL_MAX), avg(0.0), var(0.0), abw(0.0), cnt(0) {}
	};

	std::map<uint32_t, Item2> kernelItems;

	RCHECK(m_db.prepare(stmt, "SELECT duration FROM main.runs WHERE call_id = ? AND hash = ?;"));

	for(Item& item : items) {
		RCHECK(sqlite3_bind_int(stmt, 1, item.callId));
		RCHECK(sqlite3_bind_int64(stmt, 2, item.hash));
		RCHECK(sqlite3_step(stmt));
		item.duration = sqlite3_column_int64(stmt, 0);
		RCHECK(sqlite3_reset(stmt));

		Item2& it = kernelItems[item.kernelId];
		
		const double value = ((double)item.wdDuration - (double)item.duration) / 1000.0;
		
		if(value < it.min)
			it.min = value;

		if(value > it.max)
			it.max = value;

		it.avg += value;
		it.cnt++;
	}

	RCHECK(m_db.finalize(stmt));

	// calc mean
	for(auto& it : kernelItems)
		it.second.avg /= it.second.cnt;

	// calc var
	for(Item& item : items) {
		Item2& it = kernelItems[item.kernelId];

		const double value = ((double)item.wdDuration - (double)item.duration) / 1000.0;
		
		it.var += pow(value - it.avg, 2);
	}

	// calc std abw
	for(auto& it : kernelItems)
		it.second.abw = sqrt(it.second.var);

	// final results
	FILE* file = fopen(m_file, "w");
	
	L_INFO("writing output...");
	for(auto& it : kernelItems) {
		fprintf(file, "%s\t%s\t%s\t%f\t%f\t%f\t%f\t%f\t%u\n", m_gpu, m_benchmark, Static::getKernel(it.first).getName().c_str(), it.second.min, it.second.max, it.second.avg, it.second.var, it.second.abw, it.second.cnt);
	}

	fclose(file);

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
	}
}