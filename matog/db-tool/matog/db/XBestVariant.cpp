// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/shared.h>
#include <matog/log.h>
#include <matog/util.h>
#include <matog/cuda.h>
#include "../db.h"

#ifdef WIN32
#pragma warning(disable:4996)
#endif

namespace matog {
	namespace db {
//-------------------------------------------------------------------
XBestVariant::XBestVariant(int argc, char** argv) :
	m_db			(DB::getInstance()),
	m_gpuName		(0),
	m_benchmarkName	(0)
{
	if(argc < 3) {
		L_ERROR("usage: GPU BENCHMARK FILE");
		exit(1);
	}

	m_gpuName = argv[0];
	m_benchmarkName = argv[1];
	m_file = argv[2];
	
	CHECK(Static::init());
	CHECK(analyse());
}

//-------------------------------------------------------------------
Result XBestVariant::analyse(void) {
	sqlite3_stmt* stmt;

	// we expect only one gpu to be present
	const GPU* gpu = &Static::getGPU(1);

	//const Variants& variants = Static::getVariants();

	RCHECK(m_db.prepare(stmt, "SELECT module_id, kernel_id, hash FROM kernels, calls, runs WHERE kernels.id = calls.kernel_id AND calls.id = runs.call_id GROUP BY calls.id HAVING MIN(runs.duration);"));
	SQL_LOOP(stmt);
		const uint32_t moduleId	= sqlite3_column_int(stmt, 0);
		const uint32_t kernelId	= sqlite3_column_int(stmt, 1);
		const uint32_t hash		= sqlite3_column_int(stmt, 2);

		const Module& module = Static::getModule(moduleId);
		MatogConfig config[MATOG_MAX_DEGREES_PER_TARGET];
		module.getConfig(config, hash);

		TypeMap& map = m_map[kernelId];

		for(const Degree* d : module.getDegrees()) {
			if(d->getValueCount(gpu) > 1)
				map[d->getType()][(uint8_t)config[module.getDegreeIndex(d)]]++;
		}
	SQL_LEND();
	RCHECK(m_db.finalize(stmt));

	FILE* file = fopen(m_file, "a");

	for(auto& a : m_map) {
		const Kernel& kernel = Static::getKernel(a.first);

		for(auto& b : a.second) {
			if(b.first == MatogConfig::None)
				RCHECK(MatogResult::UNKNOWN_ERROR);

			for(auto& c : b.second) {
				fprintf(file, "%s\t%s\t%s\t", m_gpuName, m_benchmarkName, kernel.getName().c_str());

				switch(b.first) {
					case MatogConfig::Type::ArrayLayout: {
						const MatogArray::Layout l = (MatogArray::Layout)c.first;
						if(l == MatogArray::Layout::SOA) fprintf(file, "SoA");
						else if(l == MatogArray::Layout::AOS) fprintf(file, "AoS");
						else if(l == MatogArray::Layout::AOSOA) fprintf(file, "AoSoA");
					} break;

					case MatogConfig::Type::ArrayMemory: {
						if((uint32_t)c.first == 0) fprintf(file, "Global");
						else fprintf(file, "Texture");
					} break;

					case MatogConfig::Type::ArrayTransposition: {
						fprintf(file, "T%u", c.first);
					} break;

					case MatogConfig::Type::L1Cache: {
						const CUfunc_cache v = (CUfunc_cache)c.first;
						if(v == CU_FUNC_CACHE_PREFER_SHARED) fprintf(file, "SM");
						else if(v == CU_FUNC_CACHE_PREFER_EQUAL) fprintf(file, "EQ");
						else if(v == CU_FUNC_CACHE_PREFER_L1) fprintf(file, "L1");
					} break;

					case MatogConfig::Type::Define: {
						const uint32_t v = (uint32_t)c.first;
						if(v == 0) fprintf(file, "Shared");
						else if(v == 1) fprintf(file, "Local");
						else fprintf(file, "???");
					} break;
					
					default:
						break;
				}

				fprintf(file, "\t%u\n", c.second);
			}
		}
	}

	fclose(file);

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
	}
}