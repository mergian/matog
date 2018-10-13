// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/shared.h>
#include <matog/log.h>
#include <matog/util.h>
#include <matog/cuda.h>
#include "../db.h"

namespace matog {
	namespace db {
//-------------------------------------------------------------------
XAnalyse::XAnalyse(int argc, char** argv) :
	m_db			(DB::getInstance()),
	m_gpuName		(0),
	m_benchmarkName	(0)
{
	if(argc < 3) {
		L_ERROR("usage: GPU BENCHMARK [ALGORITHM ...]");
		exit(1);
	}

	m_gpuName = argv[0];
	m_benchmarkName = argv[1];

	CHECK(loadAllDurations());

	// TODO: analyse brute force results aswell!!!

	for(int i = 2; i < argc; i++) {
		CHECK(attach(argv[i]));
		CHECK(analyse(argv[i]));
		CHECK(detach());
		L_INFO(" ");
	}
}

//-------------------------------------------------------------------
Result XAnalyse::analyse(const char* algorithm) {
	// get duration
	sqlite3_stmt* stmt;
	
	double duration = 0.0;
	RCHECK(m_db.prepare(stmt, "SELECT SUM(duration) FROM tmp." DB_PROFILINGS ";"));
	RCHECK(m_db.exec(stmt, &DB::getDoubleCallback, &duration));

	L_INFO("%s (%fs)", algorithm, duration);

	// get other
	sqlite3_stmt* kernels, *calls, *runs, *runs2;
		
	RCHECK(m_db.prepare(kernels, "SELECT id, name FROM main." DB_KERNELS " ORDER BY name;"));
	RCHECK(m_db.prepare(calls, "SELECT id FROM main." DB_CALLS " WHERE kernel_id = ?;"));
	RCHECK(m_db.prepare(runs, "SELECT hash, duration, COUNT(*) FROM tmp." DB_RUNS " WHERE duration != 0 AND call_id = ? GROUP BY call_id HAVING MIN(duration);"));
	RCHECK(m_db.prepare(runs2, "SELECT COUNT(*) FROM tmp." DB_RUNS " WHERE call_id = ?;"));

	SQL_LOOP(kernels)
		const uint32_t kernelId	= sqlite3_column_int(kernels, 0);
		const char* kernelName	= (const char*)sqlite3_column_text(kernels, 1);

		uint32_t runCount = 0;
		uint32_t notKilledCount = 0;
		double measuredTime = 0.0;
		double realTime = 0.0;

		RCHECK(sqlite3_reset(calls));
		RCHECK(sqlite3_bind_int(calls, 1, kernelId));

		SQL_LOOP(calls)
			const uint32_t callId = sqlite3_column_int(calls, 0);

			// DURATIONS
			RCHECK(sqlite3_reset(runs));
			RCHECK(sqlite3_bind_int(runs, 1, callId));

			CallMap::const_iterator it = m_durations.find(callId);

			if(it == m_durations.end())
				RCHECK(MatogResult::UNKNOWN_ERROR);
			
			const DurationMap& map = it->second;

			SQL_LOOP(runs) 
				const uint64_t hash = sqlite3_column_int64(runs, 0);
				const uint64_t measured = sqlite3_column_int64(runs, 1);

				notKilledCount += sqlite3_column_int(runs, 2);

				DurationMap::const_iterator it = map.find(hash);

				if(it == map.end()) {
					L_ERROR("Unable to find duration from %016llX", (unsigned long long int)hash);
					RCHECK(MatogResult::UNKNOWN_ERROR);
				}

				const uint64_t real = it->second;

				measuredTime += measured / 1000000.0;
				realTime += real / 1000000.0;
			SQL_LEND()

			// RUN COUNT
			RCHECK(sqlite3_reset(runs2));
			RCHECK(sqlite3_bind_int(runs2, 1, callId));
			RCHECK(sqlite3_step(runs2));
			runCount += sqlite3_column_int(runs2, 0);
		SQL_LEND()

		L_INFO("%s: %f %f %u/%u", kernelName, measuredTime, realTime, notKilledCount, runCount);
	SQL_LEND()

	RCHECK(m_db.finalize(kernels));
	RCHECK(m_db.finalize(calls));
	RCHECK(m_db.finalize(runs));
	RCHECK(m_db.finalize(runs2));

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XAnalyse::attach(const char* algorithm) {
	std::ostringstream ss;
	ss << m_gpuName << "-" << m_benchmarkName << "-" << algorithm << ".db";

	const std::string str = ss.str();

	return m_db.attach(str.c_str(), "tmp");
}

//-------------------------------------------------------------------
Result XAnalyse::detach(void) {
	return m_db.detach("tmp");
}

//-------------------------------------------------------------------
Result XAnalyse::loadAllDurations(void) {
	L_INFO("loading durations...");

	sqlite3_stmt* stmt;

	uint32_t lastCallId = 0;
	DurationMap* map;

	RCHECK(m_db.prepare(stmt, "SELECT call_id, hash, duration FROM " DB_RUNS " ORDER BY call_id;"));
	SQL_LOOP(stmt)
		uint32_t callId = sqlite3_column_int(stmt, 0);

		// get correct map
		if(callId != lastCallId) {
			map = &m_durations[callId];
			lastCallId = callId;
		}

		// insert
		map->emplace(MAP_EMPLACE(sqlite3_column_int64(stmt, 1), sqlite3_column_int64(stmt, 2)));
	SQL_LEND()

	RCHECK(m_db.finalize(stmt));

	L_INFO("done");
	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
	}
}