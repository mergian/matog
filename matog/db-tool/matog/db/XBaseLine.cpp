// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/shared.h>
#include <matog/log.h>
#include <matog/util.h>
#include <matog/cuda.h>
#include "../db.h"

#ifdef WIN32
#pragma warning(disable: 4996)
#endif

namespace matog {
	namespace db {
//-------------------------------------------------------------------
XBaseLine::XBaseLine(int argc, char** argv) :
	m_db			(DB::getInstance()),
	m_gpuName		(0),
	m_benchmarkName	(0)
{
	if(argc < 2) {
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
Result XBaseLine::analyse(void) {
	MatogConfig soa[MATOG_MAX_DEGREES_PER_TARGET];
	MatogConfig aos[MATOG_MAX_DEGREES_PER_TARGET];
	
	sqlite3_stmt* stmt;	
	RCHECK(m_db.prepare(stmt, "SELECT CAST(duration AS FLOAT)/1000000.0 FROM runs, calls WHERE hash = ? AND kernel_id = ? AND calls.id = call_id GROUP BY call_id"));
	
	FILE* file = fopen(m_file, "a");

	for(const auto& it : Static::getKernels()) {
		const Kernel& kernel = it.second;
		const Module& module = Static::getModule(kernel.getModuleId());

		uint32_t i = 0;
		for(const Degree* d : module.getDegrees()) {
			if(d->getType() == MatogConfig::Type::ArrayLayout) {
				soa[i] = MatogArray::Layout::SOA;
				aos[i] = d->getArray()->isStruct() ? MatogArray::Layout::AOS : MatogArray::Layout::SOA;
			} else {
				soa[i] = d->getBaseline();
				aos[i] = d->getBaseline();
			}

			i++;
		}

		const uint32_t soaHash = VHash::calcHash(soa, module.getDegrees());
		const uint32_t aosHash = VHash::calcHash(aos, module.getDegrees());
		
		L_INFO("%s", kernel.getName().c_str());
		
		// soa
		double soaTime = 0.0;
		double aosTime = 0.0;

		RCHECK(sqlite3_reset(stmt));
		RCHECK(sqlite3_bind_int(stmt, 2, kernel.getId()));
		RCHECK(sqlite3_bind_int(stmt, 1, soaHash));
		SQL_LOOP(stmt)
			soaTime += sqlite3_column_double(stmt, 0);
		SQL_LEND()

		// aos
		RCHECK(sqlite3_reset(stmt));
		RCHECK(sqlite3_bind_int(stmt, 1, aosHash));
		SQL_LOOP(stmt)
			aosTime += sqlite3_column_double(stmt, 0);
		SQL_LEND()

		fprintf(file, "%s\t%s\tSOA\t%f\t\t%s\n", m_gpuName, m_benchmarkName, soaTime, kernel.getName().c_str());
		fprintf(file, "%s\t%s\tAOS\t%f\t\t%s\n", m_gpuName, m_benchmarkName, aosTime, kernel.getName().c_str());
	}

	fclose(file);

	RCHECK(m_db.finalize(stmt));

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
	}
}