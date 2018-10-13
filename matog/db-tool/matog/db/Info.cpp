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
Info::Info(int argc, char** argv) {
	Static::init();

	DB::Stmt stmt("SELECT kernel_id, MAX(C) FROM (SELECT kernel_id, COUNT(*) AS C FROM runs, calls WHERE runs.call_id = calls.id GROUP BY call_id) GROUP BY kernel_id;");

	while(stmt.step()) {
		const uint32_t kernelId	= stmt.get<uint32_t>(0);
		const uint64_t cnt		= stmt.get<uint64_t>(1);
		const Kernel& kernel	= Static::getKernel(kernelId);

		L_INFO("%s %llu/%llu", kernel.getName(), (unsigned long long int)cnt, (unsigned long long int)kernel.getConfig().count());
	}
}

//-------------------------------------------------------------------
	}
}