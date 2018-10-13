// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/shared.h>
#include <matog/log.h>
#include <matog/util.h>
#include <matog/cuda.h>
#include "Merge.h"
#include <MatogMacros.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
Merge::Merge(int argc, char** argv) {
	THROWIF(argc <2, "Usage: matog-db dest.db merge src0.db src1.db [...]");

	// attach second db as other
	DB::setFailIfDoesNotExist(false);
	
	DB& db = DB::getInstance();

	// clear all data
	Clear::all();

	// even the degrees + arrays + defines
	Degree::dbDeleteAll();
	Array ::dbDeleteAll();

	// copy from first source
	DB::Stmt("INSERT INTO " DB_ARRAYS  " SELECT FROM other." DB_ARRAYS  ";").step();
	DB::Stmt("INSERT INTO " DB_DEFINES " SELECT FROM other." DB_DEFINES ";").step();
	DB::Stmt("INSERT INTO " DB_DEGREES " SELECT FROM other." DB_DEGREES ";").step();

	// iterate databases
	for(int i = 0; i < argc; i++) {
		L_INFO("inserting %s", argv[i]);

		// attach
		db.attach(argv[i], "other");

		// clear all ids
		m_profilingIds			.clear();
		m_kernelIds				.clear();
		m_gpuIds				.clear();
		m_callIds				.clear();
		m_runIds				.clear();
		m_arrayInstanceIds		.clear();
		m_arrayInstanceTypeIds	.clear();
		m_copyIds				.clear();

		// validate if dbs can be merged
		check();

		// merge
		merge();

		// detach
		db.detach("other");
	}
}

//-------------------------------------------------------------------
void Merge::check(void) {
	L_DEBUG("checking compatibility");

	DB::Stmt stmt;

#define COMPARE(TABLE) \
	stmt("SELECT * FROM " TABLE " UNION SELECT * FROM other." TABLE " EXCEPT SELECT * FROM " TABLE " INTERSECT SELECT * FROM other." TABLE ";"); \
	THROWIF(stmt.step(), TABLE " mismatch!"); \

	COMPARE(DB_DEFINES);
	COMPARE(DB_DEGREES);
	COMPARE(DB_ARRAYS);
#undef COMPARE
}
	
//-------------------------------------------------------------------
void Merge::merge(void) {
	// no requirements
	mergeArrayInstanceTypes();
	mergeGPUs();
	mergeCubins();
	mergeModules();
	mergeMetricsLookup();
	
	// requires array instance types
	mergeArrayInstances();

	// requires array instances
	mergeArrayInstanceMeta();
	mergeCopies();

	// requires modules
	mergeKernels();
	mergeModuleDegrees();

	// requires kernels
	mergeKernelDegrees();
	mergeCubinMeta();
	mergeProfilings();

	// requires profiling
	mergeConfigs();
	mergeCalls();

	// requires calls
	mergeRuns();
	mergeCallMeta();

	// requires copy ids
	mergeEvents();
}

//-------------------------------------------------------------------
void Merge::mergeCopies(void) {
	THROWIF(m_arrayInstanceTypeIds.empty());
	L_DEBUG(DB_COPIES);

	DB::Stmt src("SELECT id, direction, src, dest  FROM other." DB_COPIES ";");
	DB::Stmt insert("INSERT INTO " DB_COPIES " (direction, src, dest) VALUES (?, ?, ?);");

	while(src.step()) {
		insert.reset();
		insert.bind(0, src.get<uint32_t>(1));
		insert.bind(1, m_arrayInstanceTypeIds[src.get<uint32_t>(2)]);
		insert.bind(2, m_arrayInstanceTypeIds[src.get<uint32_t>(3)]);

		const uint32_t newId = insert.step32();

		m_copyIds.emplace(MAP_EMPLACE(src.get<uint32_t>(0), newId));
	}
}

//-------------------------------------------------------------------
void Merge::mergeArrayInstances(void) {
	THROWIF(m_arrayInstanceTypeIds.empty());
	L_DEBUG(DB_ARRAY_INSTANCES);

	DB::Stmt src("SELECT id, type_id  FROM other." DB_ARRAY_INSTANCES ";");
	DB::Stmt insert("INSERT INTO " DB_ARRAY_INSTANCES " (type_id) VALUES (?);");

	while(src.step()) {
		insert.reset();
		insert.bind(0, m_arrayInstanceTypeIds[src.get<uint32_t>(1)]);

		const uint32_t newId = insert.step32();
		m_arrayInstanceIds.emplace(MAP_EMPLACE(src.get<uint32_t>(0), newId));
	}
}

//-------------------------------------------------------------------
void Merge::mergeArrayInstanceMeta(void) {
	TODO("port to new DB structure");
	/*
	THROWIF(m_arrayInstanceTypeIds.empty());
	L_DEBUG(DB_ARRAY_INSTANCE_META);

	DB::Stmt src("SELECT array_instance_id, dim, value  FROM other." DB_ARRAY_INSTANCE_META ";");
	DB::Stmt insert("INSERT INTO " DB_ARRAY_INSTANCE_META " (array_instance_id, dim, value) VALUES (?, ?, ?);");

	while(src.step()) {
		insert.reset();
		insert.bind(0, m_arrayInstanceIds[src.get<uint32_t>(0)]);
		insert.bind(1, src.get<uint32_t>(1));
		insert.bind(2, src.get<uint64_t>(2));
		insert.step();
	}*/
}

//-------------------------------------------------------------------
void Merge::mergeMetricsLookup(void) {
	L_DEBUG(DB_METRICS_LOOKUP);
	DB::Stmt("INSERT OR IGNORE INTO " DB_METRICS_LOOKUP " SELECT * FROM other." DB_METRICS_LOOKUP).step();
}

//-------------------------------------------------------------------
void Merge::mergeArrayInstanceTypes(void) {
	TODO("port to new DB structure");
	/*
	L_DEBUG(DB_ARRAY_INSTANCE_TYPES);

	DB::Stmt src("SELECT array_id, hash, file, line, is_device, id FROM other." DB_ARRAY_INSTANCE_TYPES ";");
	DB::Stmt dest("SELECT id FROM " DB_ARRAY_INSTANCE_TYPES " WHERE array_id = ? AND hash = ?;");
	DB::Stmt insert("INSERT INTO " DB_ARRAY_INSTANCE_TYPES " (array_id, hash, file, line, is_device) VALUES (?, ?, ?, ?, ?);");

	while(src.step()) {
		dest.reset();
		dest.bind(src);

		uint32_t new_id = 0;

		if(!dest.step()) {
			insert.bind(src);
			new_id = insert.step32();
		} else {
			new_id = dest.get<uint32_t>(0);
		}
			
		m_arrayInstanceTypeIds.emplace(MAP_EMPLACE(src.get<uint32_t>(5), new_id));
	}*/
}

//-------------------------------------------------------------------
void Merge::mergeProfilings(void) {
	L_DEBUG(DB_PROFILINGS);

	DB::Stmt src("SELECT timestamp, cmd, duration, id FROM other." DB_PROFILINGS ";");
	DB::Stmt dest("SELECT id FROM " DB_PROFILINGS " WHERE timestamp = ? AND cmd = ?;");
	DB::Stmt insert("INSERT INTO " DB_PROFILINGS " (timestamp, cmd, duration) VALUES (?, ?, ?);");

	while(src.step()) {
		dest.reset();
		dest.bind(src);

		uint32_t new_id = 0;

		if(!dest.step()) {
			L_TRACE("inserting profiling '%s // %s'", src.get<const char*>(0), src.get<const char*>(1));
			insert.bind(src);
			
			new_id = insert.step32();
		} else {
			THROW("unable to merge databases, as they contain the same profilings!");
		}
			
		m_profilingIds.emplace(MAP_EMPLACE(src.get<uint32_t>(3), new_id));
	}
}

//-------------------------------------------------------------------
void Merge::mergeConfigs(void) {
	THROWIF(m_profilingIds.empty());
	L_DEBUG(DB_CONFIG);

	DB::Stmt src("SELECT profiling_id, json FROM other." DB_CONFIG ";");
	DB::Stmt insert("INSERT OR IGNORE INTO " DB_CONFIG " (profiling_id, json) VALUES (?, ?);");

	while(src.step()) {
		insert.reset();
		insert.bind(0, m_profilingIds[src.get<uint32_t>(0)]);
		insert.bind(1, src.get<const char*>(1));
		insert.step();
	}
}

//-------------------------------------------------------------------
void Merge::mergeModules(void) {
	L_DEBUG(DB_MODULES);

	DB::Stmt src("SELECT name, source, modified, id FROM other." DB_MODULES ";");
	DB::Stmt dest("SELECT id FROM " DB_MODULES " WHERE name = ? AND source = ? AND modified = ?;");
	DB::Stmt insert("INSERT INTO " DB_MODULES " (name, source, modified) VALUES (?, ?, ?);");

	while(src.step()) {
		dest.bind(src);

		uint32_t new_id = 0;

		if(!dest.step()) {
			L_TRACE("inserting module '%s'", src.get<const char*>(0));
			new_id = insert.bind(src).step32();
		} else {
			L_TRACE("module '%s' already exists", src.get<const char*>(0));
			new_id = dest.get<uint32_t>(0);
		}
			
		m_moduleIds.emplace(MAP_EMPLACE(src.get<uint32_t>(3), new_id));
	}
}

//-------------------------------------------------------------------
void Merge::mergeKernels(void) {
	THROWIF(m_moduleIds.empty());
	L_DEBUG(DB_KERNELS);

	DB::Stmt src("SELECT name, module_id, id FROM other." DB_KERNELS ";");
	DB::Stmt dest("SELECT id FROM " DB_KERNELS " WHERE name = ? AND module_id = ?;");
	DB::Stmt insert("INSERT INTO " DB_KERNELS " (name, module_id) VALUES (?, ?);");

	while(src.step()) {
		dest.reset();
		dest.bind(0, src.get<const char*>(0));
		dest.bind(1, m_moduleIds[src.get<uint32_t>(1)]);

		uint32_t new_id = 0;

		if(!dest.step()) {
			L_TRACE("inserting kernel '%s'", src.get<const char*>(1));
			new_id = insert.bind(src).step32();
		} else {
			L_TRACE("kernel '%s' already exists", src.get<const char*>(1));
			new_id = dest.get<uint32_t>(0);
		}

		m_kernelIds.emplace(MAP_EMPLACE(src.get<uint32_t>(2), new_id));
	}
}

//-------------------------------------------------------------------
void Merge::mergeModuleDegrees(void) {
	THROWIF(m_moduleIds.empty());
	L_DEBUG(DB_MODULE_DEGREES);

	DB::Stmt src("SELECT module_id, degree_id FROM other." DB_MODULE_DEGREES ";");
	DB::Stmt insert("INSERT OR IGNORE INTO " DB_MODULE_DEGREES " (module_id, degree_id) VALUES (?, ?);");

	while(src.step()) {
		insert.reset();
		insert.bind(0, m_moduleIds[src.get<uint32_t>(0)]);
		insert.bind(1, src.get<uint32_t>(1));
		insert.step();
	}
}

//-------------------------------------------------------------------
void Merge::mergeKernelDegrees(void) {
	THROWIF(m_kernelIds.empty());
	L_DEBUG(DB_KERNEL_DEGREES);

	DB::Stmt src("SELECT kernel_id, degree_id, instance_id FROM other." DB_KERNEL_DEGREES ";");
	DB::Stmt insert("INSERT OR IGNORE INTO " DB_KERNEL_DEGREES " (kernel_id, degree_id, instance_id) VALUES (?, ?, ?);");

	while(src.step()) {
		insert.reset();
		insert.bind(0, m_kernelIds[src.get<uint32_t>(0)]);
		insert.bind(1, src.get<uint32_t>(1));
		insert.bind(2, src.get<uint32_t>(2));
		insert.step();
	}
}

//-------------------------------------------------------------------
void Merge::mergeCubins(void) {
	L_DEBUG(DB_CUBINS);
	DB::Stmt("INSERT OR IGNORE INTO " DB_CUBINS " SELECT * FROM other." DB_CUBINS ";").step();
}

//-------------------------------------------------------------------
void Merge::mergeCubinMeta(void) {
	L_DEBUG(DB_CUBIN_KERNEL_META);
	THROWIF(m_kernelIds.empty());

	DB::Stmt src("SELECT module_hash, user_hash, cc, kernel_id, static_smem, const_mem, local_mem, used_registers FROM other." DB_CUBIN_KERNEL_META ";");
	DB::Stmt insert("INSERT OR IGNORE INTO " DB_CUBIN_KERNEL_META " (module_hash, user_hash, cc, kernel_id, static_smem, const_mem, local_mem, used_registers) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");

	while(src.step()) {
		insert.bind(src);
		insert.bind(3, m_kernelIds[src.get<uint32_t>(3)]);
		insert.step();
	}
}

//-------------------------------------------------------------------
void Merge::mergeGPUs(void) {
	L_DEBUG(DB_GPUS);

	DB::Stmt src("SELECT name, global_mem, shared_mem, const_mem, cc, sm_count, max_threads_block, max_threads_sm, architecture, max_tex_width, tex_alignment, id FROM other." DB_GPUS ";");
	DB::Stmt dest("SELECT id FROM " DB_GPUS " WHERE name = ?;");
	DB::Stmt insert("INSERT INTO " DB_GPUS " (name, global_mem, shared_mem, const_mem, cc, sm_count, max_threads_block, max_threads_sm, architecture, max_tex_width, tex_alignment) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");

	while(src.step()) {
		dest.reset();
		dest.bind(0, src.get<const char*>(0));

		uint32_t new_id = 0;

		if(!dest.step()) {
			L_TRACE("inserting gpu '%s'", src.get<const char*>(0));
			insert.bind(src);
			new_id = insert.step32();
		} else {
			L_TRACE("gpu '%s' already exists", src.get<const char*>(0));
			new_id = dest.get<uint32_t>(0);
		}
			
		m_gpuIds.emplace(MAP_EMPLACE(src.get<uint32_t>(11), new_id));
	}
}

//-------------------------------------------------------------------
void Merge::mergeCalls(void) {
	THROWIF(m_profilingIds.empty());
	THROWIF(m_kernelIds.empty());
	THROWIF(m_gpuIds.empty());
	L_DEBUG(DB_CALLS);

	DB::Stmt src("SELECT profiling_id, gpu_id, kernel_id, user_flags, user_hash, bx, by, bz, tx, ty, tz, dynamic_smem, id  FROM other." DB_CALLS ";");
	DB::Stmt insert("INSERT INTO " DB_CALLS " (profiling_id, gpu_id, kernel_id, user_flags, user_hash, bx, by, bz, tx, ty, tz, dynamic_smem) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");

	while(src.step()) {
		insert.bind(src);
		insert.bind(0, m_profilingIds	[src.get<uint32_t>(0)]);
		insert.bind(1, m_gpuIds			[src.get<uint32_t>(1)]);
		insert.bind(2, m_kernelIds		[src.get<uint32_t>(2)]);

		const uint32_t newId = insert.step32();
		m_callIds.emplace(MAP_EMPLACE(src.get<uint32_t>(12), newId));
	}
}

//-------------------------------------------------------------------
void Merge::mergeRuns(void) {
	THROWIF(m_callIds.empty());
	L_DEBUG(DB_RUNS);

	DB::Stmt src("SELECT call_id, hash, static_smem, dynamic_smem, duration, requested_cache, executed_cache, occupancy, id FROM other." DB_RUNS ";");
	DB::Stmt insert("INSERT INTO " DB_RUNS " (call_id, hash, static_smem, dynamic_smem, duration, requested_cache, executed_cache, occupancy) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");

	while(src.step()) {
		insert.bind(src);
		insert.bind(0, m_callIds[src.get<uint32_t>(0)]);

		const uint32_t newId = insert.step32();
		m_runIds.emplace(MAP_EMPLACE(src.get<uint32_t>(8), newId));
	}
}

//-------------------------------------------------------------------
void Merge::mergeCallMeta(void) {
	THROWIF(m_callIds.empty());
	THROWIF(m_arrayInstanceIds.empty());
	L_DEBUG(DB_CALL_META);

	DB::Stmt src("SELECT call_id, array_instance_id, call_instance_id, mode FROM other." DB_CALL_META ";");
	DB::Stmt insert("INSERT INTO " DB_CALL_META " (call_id, array_instance_id, call_instance_id, mode) VALUES (?, ?, ?, ?);");

	while(src.step()) {
		insert.bind(src);
		insert.bind(0, m_callIds			[src.get<uint32_t>(0)]);
		insert.bind(1, m_arrayInstanceIds	[src.get<uint32_t>(1)]);
		insert.step();
	}
}

//-------------------------------------------------------------------
void Merge::mergeEvents(void) {
	THROWIF(m_callIds.empty());
	THROWIF(m_copyIds.empty());
	THROWIF(m_arrayInstanceIds.empty());
	THROWIF(m_profilingIds.empty());
	L_DEBUG(DB_EVENTS);

	DB::Stmt src("SELECT profiling_id, type, event_id FROM other." DB_EVENTS ";");
	DB::Stmt insert("INSERT INTO " DB_EVENTS " (profiling_id, type, event_id) VALUES (?, ?, ?);");

	while(src.step()) {
		insert.reset();
		insert.bind(0, m_profilingIds[src.get<uint32_t>(0)]);
		insert.bind(1, src.get<uint32_t>(1));
		
		switch(src.get<Profiling::EventType>(1)) {
		case Profiling::EventType::Alloc:
			insert.bind(2, m_arrayInstanceIds	[src.get<uint32_t>(2)]);
			break;
		case Profiling::EventType::Call:
			insert.bind(2, m_callIds			[src.get<uint32_t>(2)]);
			break;
		case Profiling::EventType::Copy:
			insert.bind(2, m_copyIds			[src.get<uint32_t>(2)]);
			break;
		default:
			THROW();
		}
		
		insert.step();
	}
}

//-------------------------------------------------------------------
	}
}