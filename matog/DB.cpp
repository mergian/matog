// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <matog/DB.h>
#include <matog/db/tables.h>
#include <matog/util/Hash.h>
#include <matog/util/SQLStmt.h>
#include <sstream>

#ifdef WIN32
#pragma warning(disable: 4996)
#endif

namespace matog {
//-------------------------------------------------------------------
std::string	db_dbFile			= DB::getDefaultFileName();
bool	db_setupNewDB			= false;
bool	db_failIfDoesNotExist	= false;
DB*		DB::s_instance			= 0;

//-------------------------------------------------------------------
const char* DB_CREATE_TABLES = 
	// DEGREES
	"CREATE TABLE " DB_DEGREES " (id INTEGER PRIMARY KEY AUTOINCREMENT, array_id INTEGER NOT NULL, name TEXT UNIQUE NOT NULL, type INTEGER NOT NULL, value_count INTEGER NOT NULL, is_compiler BOOLEAN NOT NULL, is_function BOOLEAN NOT NULL, is_shared BOOLEAN NOT NULL, is_root BOOLEAN NOT NULL);"
	// INSERT L1CACHE
	"INSERT INTO " DB_DEGREES " (name, array_id, type, value_count, is_compiler, is_function, is_shared, is_root) VALUES ('L1Cache', 0, 1, 3, 0, 1, 1, 0);"
	
	// DEFINES
	"CREATE TABLE " DB_DEGREE_DEFINES " (degree_id INTEGER NOT NULL, ordering INTEGER NOT NULL, key TEXT NOT NULL, value INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_DEGREE_DEFINES "_INDEX ON " DB_DEGREE_DEFINES " (degree_id ASC, ordering ASC);"

	// RANGES
	"CREATE TABLE " DB_DEGREE_RANGES " (degree_id INTEGER UNIQUE NOT NULL, min FLOAT NOT NULL, max FLOAT NOT NULL, step FLOAT NOT NULL);"

	// MODULE
	"CREATE TABLE " DB_MODULES " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE NOT NULL, source TEXT UNIQUE NOT NULL, modified INTEGER NOT NULL);"

	// MODULE_KERNELS
	"CREATE TABLE " DB_KERNELS " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, module_id INTEGER NOT NULL);"
	
	"CREATE TABLE " DB_KERNEL_DEGREES " (kernel_id INTEGER NOT NULL, degree_id INTEGER NOT NULL, instance_id INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_KERNEL_DEGREES "_INDEX ON " DB_KERNEL_DEGREES " (kernel_id ASC, degree_id ASC, instance_id ASC);"
	
	"CREATE TABLE " DB_KERNEL_HINTS_RW " (kernel_name TEXT NOT NULL, array_id INTEGER NOT NULL, param_id INTEGER NOT NULL, mode INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_KERNEL_HINTS_RW "_INDEX ON " DB_KERNEL_HINTS_RW " (kernel_name ASC, array_id ASC, param_id ASC);"

	"CREATE TABLE " DB_KERNEL_HINTS_REF " (kernel_name TEXT NOT NULL, array_id INTEGER NOT NULL, param_id INTEGER NOT NULL, dim INTEGER NOT NULL, src_array_id INTEGER NOT NULL, src_param_id INTEGER NOT NULL, src_dim INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_KERNEL_HINTS_REF "_INDEX ON " DB_KERNEL_HINTS_REF " (kernel_name ASC, array_id ASC, param_id ASC, dim ASC);"
	
	"CREATE TABLE " DB_KERNEL_HINTS_REF_INDEX " (kernel_name TEXT NOT NULL, array_id INTEGER NOT NULL, param_id INTEGER NOT NULL, idx INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_KERNEL_HINTS_REF_INDEX "_INDEX ON " DB_KERNEL_HINTS_REF_INDEX " (kernel_name ASC, array_id ASC, param_id ASC);"

	"CREATE TABLE " DB_KERNEL_HINTS_COUNTS " (kernel_name TEXT NOT NULL, array_id INTEGER NOT NULL, param_id INTEGER NOT NULL, dim INTEGER NOT NULL, value INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_KERNEL_HINTS_COUNTS "_INDEX ON " DB_KERNEL_HINTS_COUNTS " (kernel_name ASC, array_id ASC, param_id ASC, dim ASC);"

	// MODULE_ARRAYS
	"CREATE TABLE " DB_MODULE_DEGREES " (module_id INTEGER NOT NULL, degree_id INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_MODULE_DEGREES "_INDEX ON " DB_MODULE_DEGREES " (module_id ASC, degree_id ASC);"

	// CUBINS
	"CREATE TABLE " DB_CUBINS " (module_id INTEGER NOT NULL, module_hash INTEGER NOT NULL, user_hash INTEGER NOT NULL, arch INTEGER NOT NULL, cc INTEGER NOT NULL, code BLOB NOT NULL);"

	// CUBIN_META
	"CREATE TABLE " DB_CUBINS_INFO " (module_hash INTEGER NOT NULL, user_hash INTEGER NOT NULL, cc INTEGER NOT NULL, kernel_id INTEGER NOT NULL, static_smem INTEGER NOT NULL, const_mem INTEGER NOT NULL, local_mem INTEGER NOT NULL, used_registers INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_CUBINS_INFO "_INDEX ON " DB_CUBINS_INFO " (module_hash ASC, user_hash ASC, cc ASC, kernel_id ASC, cc ASC);"

	// CUBIN_FLAGS
	"CREATE TABLE " DB_CUBINS_FLAGS " (user_hash INTEGER UNIQUE NOT NULL, user_flags TEXT NOT NULL);"

	// GPUS
	"CREATE TABLE " DB_GPUS " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE NOT NULL, global_mem INTEGER NOT NULL, shared_mem INTEGER NOT NULL, const_mem INTEGER NOT NULL, cc INTEGER NOT NULL, sm_count INTEGER NOT NULL, max_threads_block INTEGER NOT NULL, max_threads_sm INTEGER NOT NULL, architecture INTEGER NOT NULL, max_tex_width INTEGER NOT NULL, tex_alignment INTEGER NOT NULL);"

	// PROFILINGS
	"CREATE TABLE " DB_PROFILINGS " (id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TEXT NOT NULL, cmd TEXT NOT NULL, duration FLOAT DEFAULT 0, profiler INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_PROFILINGS "_INDEX ON " DB_PROFILINGS " (timestamp ASC, cmd ASC);"

	// CALLS
	"CREATE TABLE " DB_CALLS " (call_id INTEGER UNIQUE NOT NULL, kernel_id INTEGER NOT NULL, user_hash INTEGER NOT NULL, dynamic_smem INTEGER NOT NULL);"

	// RUNS
	"CREATE TABLE " DB_RUNS " (id INTEGER PRIMARY KEY AUTOINCREMENT, call_id INTEGER NOT NULL, hash INTEGER NOT NULL, static_smem INTEGER NOT NULL DEFAULT(0), "
		"dynamic_smem INTEGER NOT NULL DEFAULT(0), duration INTEGER NOT NULL DEFAULT(0), requested_cache INTEGER NOT NULL DEFAULT(0), executed_cache INTEGER NOT NULL DEFAULT(0), "
		"occupancy FLOAT NOT NULL DEFAULT(0.0));"
	"CREATE INDEX " DB_RUNS "_INDEX ON " DB_RUNS " (call_id ASC);"

	// METRICS
	"CREATE TABLE " DB_METRICS " (run_id INTEGER NOT NULL, metric_id INTEGER NOT NULL, value_int INTEGER NOT NULL, value_flt FLOAT NOT NULL, kind INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_METRICS "_INDEX ON " DB_METRICS " (run_id ASC, metric_id ASC);"
	"CREATE TABLE " DB_METRICS_LOOKUP " (metric_id INTEGER UNIQUE NOT NULL, name TEXT NOT NULL);"

	// CONFIG
	"CREATE TABLE " DB_CONFIG " (profiling_id INTEGER UNIQUE NOT NULL, json TEXT NOT NULL);"

	// ARRAYS
	"CREATE TABLE " DB_ARRAYS " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE NOT NULL, ifndef TEXT UNIQUE NOT NULL, type INTEGER NOT NULL, "
		"is_struct BOOLEAN NOT NULL, is_cube BOOLEAN NOT NULL, is_32bitcount BOOLEAN NOT NULL, vdim_count INTEGER NOT NULL, root_dim_count INTEGER NOT NULL, root_sizes BLOB NOT NULL, root_ptr_count INTEGER NOT NULL);"

	// EVENTS
	"CREATE TABLE " DB_EVENTS " (id INTEGER PRIMARY KEY AUTOINCREMENT, profiling_id INTEGER NOT NULL, decision_id INTEGER NOT NULL);"
	"CREATE TABLE " DB_EVENT_DATA " (event_id INTEGER NOT NULL, source_event_id INTEGER NOT NULL, param_id INTEGER NOT NULL, decision_id INTEGER NOT NULL, "
		"decision_offset INTEGER NOT NULL, dim INTEGER NOT NULL, value FLOAT NOT NULL);"
	
	// DECISIONS
	"CREATE TABLE " DB_DECISIONS " (id INTEGER PRIMARY KEY AUTOINCREMENT, hash INTEGER NOT NULL, array_id INTEGER NOT NULL, kernel_id INTEGER NOT NULL, gpu_id INTEGER NOT NULL, "
		"file TEXT, line INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_DECISIONS "_INDEX ON " DB_DECISIONS " (hash ASC, array_id ASC, kernel_id ASC);"

	// MODELS
	"CREATE TABLE " DB_MODELS " (decision_id INTEGER UNIQUE NOT NULL, type INTEGER NOT NULL, model BLOB NOT NULL);"

	"CREATE TABLE " DB_MODEL_FIELDS " (decision_id INTEGER NOT NULL, ordering INTEGER NOT NULL, meta_decision_id INTEGER NOT NULL, meta_decision_offset INTEGER NOT NULL, dim INTEGER NOT NULL);"
	"CREATE UNIQUE INDEX " DB_MODEL_FIELDS "_INDEX ON " DB_MODEL_FIELDS " (decision_id ASC, ordering ASC);"
	;

const char* DB_DOES_VERSION_MATCH	= "SELECT COUNT(name) FROM sqlite_master WHERE type=\"table\" AND name = ?;";

//-------------------------------------------------------------------
const char* DB::getDefaultFileName(void) {
	return "matog/matog.db";
}

//-------------------------------------------------------------------
DB::DB(void) : SQLiteDB(db_dbFile.c_str(), db_failIfDoesNotExist) {
	if(db_setupNewDB) {
		deleteDBFile();
		openDB();
		createTables();
	} else {
		// open db
		openDB();

		// create tables if necessary
		checkTables();
	}
}

//-------------------------------------------------------------------
DB::~DB(void) {
}

//-------------------------------------------------------------------
void DB::checkTables(void) {
	// get version
	uint32_t version = 0;

	char tmp[8 + 9];
	sprintf(tmp, "VERSION_%08X", util::Hash::crc32(DB_CREATE_TABLES));
	
	util::SQLStmt stmt(DB_DOES_VERSION_MATCH, *this);
	stmt.bind(0, (const char*)tmp);

	THROWIF(!stmt.step());
	version = stmt.get<uint32_t>(0);

	// check version
	if(version != 1) {
		util::SQLStmt stmt("SELECT name FROM sqlite_master WHERE name LIKE 'VERSION_%';", *this);
		THROWIF(!stmt.step(), "unable to determine database version");
		
		const char* ver = stmt.get<const char*>(0);

		std::ostringstream ss;
		ss << "expected " << tmp << " but is " << ver;
		THROW("DB_VERSION_MISMATCH", ss.str().c_str());
	}
}

//-------------------------------------------------------------------
void DB::createTables(void) {
	std::istringstream ss(DB_CREATE_TABLES);
	std::string line;
	std::getline(ss, line, ';');

	util::SQLStmt stmt(*this);

	while(!line.empty()) {
		stmt(line.c_str()).step();
		line.clear();
		std::getline(ss, line, ';');
	}

	const uint32_t hash = util::Hash::crc32(DB_CREATE_TABLES);
	char tmp[35 + 9];
	sprintf(tmp, "CREATE TABLE VERSION_%08X (id INTEGER);", hash);
	util::SQLStmt(tmp, *this).step();

	L_DEBUG("Creating SQL tables Version: %08X", hash);
}

//-------------------------------------------------------------------
void DB::setSetupNewDB(const bool setupNewDB) {
	db_setupNewDB = setupNewDB;
}

//-------------------------------------------------------------------
void DB::setFailIfDoesNotExist(const bool failIfDoesNotExist) {
	db_failIfDoesNotExist = failIfDoesNotExist;
}

//-------------------------------------------------------------------
void DB::setDBFile(const char* dbfile) {
	THROWIF(isInitialized(), "DB_ALREADY_INITIALIZED");
	db_dbFile = dbfile;
}

//-------------------------------------------------------------------
}