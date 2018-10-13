// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/shared.h>
#include <matog/log.h>
#include <matog/util.h>
#include <matog/cuda.h>
#include "../db.h"
#include <cstdio>
#include <MatogMacros.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
Predict::Predict(int argc, char** argv) {
	Static::init();

	DB::Stmt stmt("SELECT id, kernel_id FROM " DB_CALLS ";");

	while(stmt.step())
		Item(stmt.get<uint32_t>(0), stmt.get<uint32_t>(1));
}

//-------------------------------------------------------------------
Predict::Item::Item(const uint32_t callId, const uint32_t kernelId) : 
	m_kernel(Static::getKernel(kernelId)),
	m_module(Static::getModule(m_kernel.getModuleId())),
	m_callId(callId)
{
	L_INFO("%u: %s::%s", m_callId, m_module.getName(), m_kernel.getName());

	getTimes();
	predict();
}

//-------------------------------------------------------------------
void Predict::Item::getTimes(void) {
	DB::Stmt stmt("SELECT calls.call_event_id AS Id, kernels.name AS KernelName, runs.duration AS Time, calls.bx, calls.by, calls.bz, calls.tx, calls.ty, calls.tz FROM kernels, calls, runs WHERE kernels.id = calls.[kernel_id] AND calls.id = runs.[call_id];");
	DB::Stmt meta("SELECT calls.call_event_id AS Id, arrays.[name] AS Array, array_instance_types.[file] AS File, array_instance_types.[line] AS Line, array_instance_meta.[dim] AS Dim, array_instance_meta.[value] AS Value  FROM calls, call_meta, array_instances, array_instance_meta, array_instance_types, arrays WHERE calls.[id] = call_meta.[call_id] AND call_meta.[array_instance_id] = array_instance_meta.[array_instance_id] AND array_instances.[id] = array_instance_meta.[array_instance_id] AND array_instances.[type_id] = array_instance_types.[id] AND array_instance_types.[array_id] = arrays.id AND calls_id = ?;");
	
	struct myCall {
		std::string name;
		std::vector<uint64_t> meta;
	};

	std::list<myCall> calls;

	while(stmt.step()) {
		uint32_t callId = stmt.get<uint32_t>(0);
		meta.bind(0, callId);

		calls.emplace_back();

		myCall& ref = calls.back();
		ref.name = stmt.get<const char*>(1);
	
		while(meta.step()) {
			ref.meta.emplace_back(stmt.get<uint64_t>(5));
		}
	}
}

//-------------------------------------------------------------------
void Predict::Item::predict(void) {
#ifdef WIN32
#pragma warning(disable: 4996)
#endif

	FILE* file = 0;
	std::ostringstream ss;
	ss << m_kernel.getName() << "_" << m_callId << ".txt";

	file = fopen(ss.str().c_str(), "w");
	THROWIF(file == 0, "IO_EXCEPTION", "Unable to open file");

	fprintf(file, "Hash\tDuration\tNDuration\tPredicted\tNPredicted\n");

	const auto& items = m_kernel.getDegrees();

	for(uint64_t hash = 0; hash < ((const Config::Data&)m_kernel).count(); hash++) {
		// skip if not in brute force
		if(m_times.find(hash) == m_times.end())
			continue;

		// init stuff
		double duration = 0;
		double duration2 = 0;

		// get config
		m_kernel.getConfig(m_config, hash);

		// calc base class
		memset(m_tmpConfig, 0, sizeof(uint32_t) * m_kernel.getDegrees().size());
		for(const auto& item : m_shared) {
			const uint32_t index	= item.second;
			m_tmpConfig[index]		= m_config[index];
		}

		const uint64_t baseHash = m_kernel.calcHash(m_tmpConfig);

		// skip if base is not available, as this is an indicator for "does not work"
		const Run& base	= getRun(baseHash);
		
		// P(X) = T(B) + ...
		duration += base.duration;
		duration2 += base.duration / base.clock;

		// iterate all independent degrees
		for(uint32_t index = 0; index < items.size(); index++) {
			const Degree& degree = *items[index].getDegree();

			if(degree.isShared())
				continue;

			m_tmpConfig[index] = m_config[index];

			const uint64_t supportHash	= m_kernel.calcHash(m_tmpConfig);
			const Run& support			= getRun(supportHash);

			// P(X) = ... + SUM(T(S) - T(B))
			duration += support.duration - base.duration;
			duration2 += support.duration / support.clock - base.duration / base.clock;

			// reset value to baseline
			m_tmpConfig[index] = 0;
		}

		const Run& real = getRun(hash);
		fprintf(file, "%llu\t%f\t%f\t%f\t%f\n", (unsigned long long int)hash, real.duration, real.duration / real.clock, duration, duration2);
	}

	fclose(file);
}

//-------------------------------------------------------------------
const Predict::Item::Run& Predict::Item::getRun(const uint64_t hash) {
	const auto it = m_times.find(hash);
	THROWIF(it == m_times.end());
	return it->second;
}

//-------------------------------------------------------------------
	}
}