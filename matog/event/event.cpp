// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/internal.h>
#include <matog/db/tables.h>
#include <matog/db/Stmt.h>
#include <matog/log.h>
#include <matog/util/IO.h>

using matog::db::Stmt;

namespace matog {
	namespace event {
// TODO: LARGE PARTS OF THIS FILE CAN BE MOVED TO matog::runtime!

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL printSummary(const uint32_t id) {
	Stmt stmt(
	"SELECT " DB_KERNELS ".name, COUNT(A.time), SUM(A.time), MIN(A.time), MAX(A.time), AVG(A.time) FROM "
		DB_KERNELS ", "
		"(SELECT MIN(duration) AS time, " DB_CALLS ".kernel_id AS kernel_id "
			"FROM "
			DB_RUNS ", "
			DB_CALLS " "
			"WHERE "
			DB_RUNS ".call_id = " DB_CALLS ".call_id AND "
			DB_CALLS ".call_id IN (SELECT id FROM " DB_EVENTS " WHERE profiling_id = ?) AND "
			DB_RUNS ".duration != 0 " // excludes killed runs
			"GROUP BY "
			DB_CALLS ".call_id "
		") AS A "
		"WHERE A.kernel_id = " DB_KERNELS ".id "
		"GROUP BY " DB_KERNELS ".name ORDER BY SUM(A.time) DESC;");
	stmt.bind(0, id);

	C_INFO(log::GREEN, "### Profiling Summary (in ms)");
	while(stmt.step()) {
		L_INFO("%-30s Calls: %4u Sum: %7.2f Min: %7.2f Max: %7.2f Avg: %7.2f", 
			stmt.get<const char*>(0),
			stmt.get<uint32_t>(1),
			stmt.get<double>(2) / 1000000.0,
			stmt.get<double>(3) / 1000000.0,
			stmt.get<double>(4) / 1000000.0,
			stmt.get<double>(5) / 1000000.0
		);
	}
	C_INFO(log::GREEN, "###");
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbDeleteAll(void) {
	dbDeleteAllProfilings();
	dbDeleteAllCalls();
	dbDeleteAllMetrics();
	dbDeleteAllRuns();
	dbDeleteAllEvents();
}

//-------------------------------------------------------------------
uint32_t MATOG_INTERNAL_DLL dbInsertProfiling(const event::ProfilingAlgorithm profiler) {
	const char* cmd = util::IO::getCommandline();
	Stmt stmt("INSERT INTO " DB_PROFILINGS " (timestamp, cmd, profiler) VALUES (DATETIME(\"NOW\"), ?, ?);");
	stmt.bind(0, cmd);
	stmt.bind(1, profiler);
	const uint32_t id = stmt.step32();
	FREE_A(cmd);
	return id;
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbDeleteAllProfilings(void) {
	Stmt("DELETE FROM sqlite_sequence WHERE NAME = '" DB_PROFILINGS "';").step();
	Stmt("DELETE FROM " DB_PROFILINGS ";").step();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbUpdateProfilingDuration(const uint32_t profilingId) {
	Stmt("UPDATE " DB_PROFILINGS " SET duration = ? WHERE id = ?;")
		.bind(0, log::getSeconds())
		.bind(1, profilingId)
		.step();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbInsertMetric(const uint32_t runId, const CUpti_MetricID metricId, CUpti_MetricValue value, CUpti_MetricValueKind kind) {
	// add metric name to DB if necessary
	if(metricId != 0) {
		// get name
		char name[128];
		size_t nameSize = sizeof(name) - 1;
		CHECK(cuptiMetricGetAttribute(metricId, CUPTI_METRIC_ATTR_NAME, &nameSize, name));
		Stmt("INSERT OR IGNORE INTO " DB_METRICS_LOOKUP " (metric_id, name) VALUES (?, ?);")
			.bind(0, metricId)
			.bind(1, (const char*)name)
			.step();
	}

	// insert metric value
	Stmt("INSERT INTO " DB_METRICS " (run_id, metric_id, value_int, value_flt, kind) VALUES (?, ?, ?, ?, ?);")
		.bind(0, runId)
		.bind(1, metricId)
		.bind(2, value.metricValueInt64)
		.bind(3, value.metricValueDouble)
		.bind(4, (uint32_t)kind)
		.step();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbDeleteMetrics(const uint32_t runId) {
	Stmt("DELETE FROM " DB_METRICS " WHERE run_id = ?;").bind(0, runId).step();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbDeleteAllMetrics(void) {
	Stmt("DELETE FROM " DB_METRICS_LOOKUP ";").step();
	Stmt("DELETE FROM " DB_METRICS ";").step();
}

//-------------------------------------------------------------------
uint32_t MATOG_INTERNAL_DLL dbInsertEvent(const uint32_t profilingId, const uint32_t decisionId) {
	return Stmt("INSERT INTO " DB_EVENTS " (profiling_id, decision_id) VALUES (?, ?);")
		.bind(0, profilingId)
		.bind(1, decisionId)
		.step32();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbDeleteAllEvents(void) {
	Stmt("DELETE FROM sqlite_sequence WHERE NAME = '" DB_EVENTS "';").step();
	Stmt("DELETE FROM sqlite_sequence WHERE NAME = '" DB_DECISIONS "';").step();
	Stmt("DELETE FROM " DB_EVENTS ";").step();
	Stmt("DELETE FROM " DB_EVENT_DATA ";").step();
	Stmt("DELETE FROM " DB_DECISIONS ";").step();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbInsertEventData(const uint32_t eventId, const uint32_t sourceEventId, const uint32_t paramId, const uint32_t decisionId, const uint32_t decisionOffset, const uint32_t dim, const float value) {
	// TODO: Improve DB performance of this function. Reinit Stmt again and again is not very fast...
	Stmt("INSERT INTO " DB_EVENT_DATA " (event_id, source_event_id, param_id, decision_id, decision_offset, dim, value) VALUES (?, ?, ?, ?, ?, ?, ?);")
		.bind(0, eventId)
		.bind(1, sourceEventId)
		.bind(2, paramId)
		.bind(3, decisionId)
		.bind(4, decisionOffset)
		.bind(5, dim)
		.bind(6, value)
		.step();
}

//-------------------------------------------------------------------
uint32_t MATOG_INTERNAL_DLL dbInsertDecision(const uint32_t hash, const uint32_t arrayId, const uint32_t kernelId, const uint32_t gpuId, const char* file, const int line) {
	Stmt stmt("SELECT id FROM " DB_DECISIONS " WHERE hash = ? AND array_id = ? AND kernel_id = ? AND gpu_id = ? LIMIT 1;");
	stmt.bind(0, hash);
	stmt.bind(1, arrayId);
	stmt.bind(2, kernelId);
	stmt.bind(3, gpuId);

	if(stmt.step())
		return stmt.get<uint32_t>(0);
	
	// insert instance type
	return Stmt("INSERT INTO " DB_DECISIONS " (hash, array_id, kernel_id, gpu_id, file, line) VALUES (?, ?, ?, ?, ?, ?);")
		.bind(0, hash)
		.bind(1, arrayId)
		.bind(2, kernelId)
		.bind(3, gpuId)
		.bind(4, file)
		.bind(5, line)
		.step32();
}

//-------------------------------------------------------------------
uint32_t MATOG_INTERNAL_DLL dbSelectDecisionId(const uint32_t hash, const uint32_t arrayId, const uint32_t kernelId, const uint32_t gpuId) {
	Stmt stmt("SELECT id FROM " DB_DECISIONS " WHERE hash = ? AND array_id = ? AND kernel_id = ? AND gpu_id = ? LIMIT 1;");
	stmt.bind(0, hash);
	stmt.bind(1, arrayId);
	stmt.bind(2, kernelId);
	stmt.bind(3, gpuId);

	THROWIF(!stmt.step());
	return stmt.get<uint32_t>(0);
}

//-------------------------------------------------------------------
uint32_t MATOG_INTERNAL_DLL dbInsertCall(const uint32_t profilingId, const uint32_t decisionId, const uint32_t gpuId, const uint32_t kernelId, const uint32_t userHash, const uint3 blocks, const uint3 threads, const uint32_t dynamicSharedMem) {
	// insert event
	const uint32_t id = dbInsertEvent(profilingId, decisionId);
	
	// insert call
	Stmt("INSERT INTO " DB_CALLS " (call_id, kernel_id, user_hash, dynamic_smem) VALUES (?, ?, ?, ?);")
		.bind(0, id)
		.bind(1, kernelId)
		.bind(2, userHash)
		.bind(3, dynamicSharedMem)
		.step();

	// insert meta
	dbInsertEventData(id, 0, 0, decisionId, 0, 0, (float)blocks.x);
	dbInsertEventData(id, 0, 0, decisionId, 0, 1, (float)blocks.y);
	dbInsertEventData(id, 0, 0, decisionId, 0, 2, (float)blocks.z);
	dbInsertEventData(id, 0, 0, decisionId, 0, 3, (float)threads.x);
	dbInsertEventData(id, 0, 0, decisionId, 0, 4, (float)threads.y);
	dbInsertEventData(id, 0, 0, decisionId, 0, 5, (float)threads.z);

	return id;
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbDeleteAllCalls(void) {
	Stmt("DELETE FROM " DB_CALLS ";").step();
}

//-------------------------------------------------------------------
uint32_t MATOG_INTERNAL_DLL dbInsertRun(const uint32_t callId, const uint64_t hash) {
	return Stmt("INSERT INTO " DB_RUNS " (call_id, hash) VALUES (?, ?);")
		.bind(0, callId)
		.bind(1, hash)
		.step32();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbDeleteAllRuns(void) {
	Stmt("DELETE FROM sqlite_sequence WHERE NAME = '" DB_RUNS "';").step();
	Stmt("DELETE FROM " DB_RUNS ";").step();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbDeleteRun(const uint32_t runId) {
	Stmt("DELETE FROM " DB_RUNS " WHERE id = ?;").bind(0, runId).step();
	dbDeleteMetrics(runId);
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbSetRunData(const uint32_t runId, const uint64_t duration, const uint32_t requested, const uint32_t executed) {
	Stmt("UPDATE " DB_RUNS " SET duration = ?, requested_cache = ?, executed_cache = ? WHERE id = ?;")
		.bind(0, duration)
		.bind(1, requested)
		.bind(2, executed)
		.bind(3, runId)
		.step();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbSetRunSharedMemory(const uint32_t runId, const uint32_t staticMem, const uint32_t dynamicMem) {
	Stmt("UPDATE " DB_RUNS " SET static_smem = ?, dynamic_smem = ? WHERE id = ?;")
		.bind(0, staticMem)
		.bind(1, dynamicMem)
		.bind(2, runId)
		.step();
}

//-------------------------------------------------------------------
void MATOG_INTERNAL_DLL dbSetRunOccupancy(const uint32_t runId, const double occupancy) {
	Stmt("UPDATE " DB_RUNS " SET occupancy = ? WHERE id = ?;")
		.bind(0, occupancy)
		.bind(1, runId)
		.step();
}

//-------------------------------------------------------------------
	}
}