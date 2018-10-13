// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Misc.h"
#include <matog/Static.h>
#include <matog/Options.h>
#include <matog/cuda/CUBINStore.h>
#include <matog/util.h>
#include <matog/log.h>
#include <matog/DB.h>
#include <matog/db/Stmt.h>
#include <matog/Kernel.h>
#include <matog/Degree.h>
#include <matog/Variant.h>
#include <matog/variant/Type.h>
#include <matog/degree/Type.h>
#include <unordered_map>
#include <matog/macros.h>
#include <tbb/tbb.h>

using matog::DB;
using matog::db::Stmt;

namespace matog {
	namespace misc {
//-------------------------------------------------------------------
Misc::Misc(int argc, char** argv) {
	// init db
	initDB(argc, argv);

	/*for (const auto& it : Static::getKernels()) {
		const Kernel& kernel = it.second;
		const Variant& variant = kernel.getVariant();

		printf("%s\n", kernel.getName());

		for(const auto& dit : variant.items()) {
			printf("%s --> %u --> %llu / %u\n", dit.getDegree()->getName(), dit.getParamId(), variant.mul(dit), dit.getDegree()->getValueCount());
		}

		printf("\n\n");
	}*/

	/*
	for(const auto& it : Static::getKernels()) {
		const Kernel& kernel = it.second;
		const Variant& variant = kernel.getVariant();
		
		uint64_t total = 1;
		uint64_t matogMul = 1;
		uint64_t matogSum = 1;
		
		for(const auto& item : variant.items()) {
			const Degree& degree = *item.getDegree();
			const uint32_t paramId = item.getParamId();
			
			uint32_t count = degree.getValueCount();

			if(degree.getType() == degree::Type::ArrayMemory) {
				RWMode mode = kernel.getRWMode(degree.getArray()->getId(), paramId);
				
				if(mode != RWMode::READ_ONLY && mode != RWMode::READ_ONCE)
					count = 1;
			}
			
			total *= count;
			
			if(degree.isShared())
				matogMul *= count;
			else
				matogSum += count - 1;
		}
		uint64_t matog = matogMul * matogSum;
		printf("%s\t%llu\t%llu\t%llu\n", kernel.getName(), (unsigned long long int)variant.count(), (unsigned long long int)total, (unsigned long long int)matog);
	}
	*/
	// predict
	//predict();
	appendixA();
}

//-------------------------------------------------------------------
void Misc::appendixA(void) {
	Stmt call("SELECT call_id, kernel_id FROM calls;");
	Stmt prof("SELECT duration FROM profiling;");
	Stmt run("SELECT hash, duration FROM runs WHERE call_id = ?");
	Stmt cnt("SELECT COUNT(*) FROM runs WHERE call_id = ?");

	// for all calls
	std::unordered_map<variant::Hash, double> times;

	typedef std::pair<double, uint64_t> Pair;
	typedef tbb::enumerable_thread_specific<Pair> dType;
	dType overallThreads(std::make_pair(0.0, 0));
	uint64_t profCnt = 0;

	while (call.step()) {
		const uint32_t callId = call.get<uint32_t>(0);
		const Kernel& kernel = Static::getKernel(call.get<uint32_t>(1));
		const Variant& variant = kernel.getVariant();

		run.reset();
		cnt.reset();

		run.bind(0, callId);
		cnt.bind(0, callId);

		cnt.step();
		const uint32_t runCnt = cnt.get<uint32_t>(0);

		times.clear();
		times.reserve(runCnt);
		profCnt += runCnt;

		// store times
		while (run.step()) {
			const variant::Hash hash = run.get<variant::Hash>(0);
			const double duration = run.get<uint64_t>(1) / 1000000.0;
			times.emplace(MAP_EMPLACE(hash, duration));
		}

		// iterate times
		const variant::Hash hashCnt = variant.count();
		L_INFO("%s --> %llu", kernel.getName(), hashCnt);

		tbb::parallel_for(tbb::blocked_range<variant::Hash>(0, hashCnt), [&times, &overallThreads, &variant](const tbb::blocked_range<variant::Hash>& range) {
			dType::reference overall = overallThreads.local();
			printf(".");

			for(variant::Hash hash = range.begin(); hash < range.end(); hash++) {
				if (times.find(hash) != times.end())
					continue;

				const variant::Hash baseHash = variant.convert(hash, variant::Type::Shared);
				const auto baseIt = times.find(baseHash);

				if (hash == baseHash || baseIt == times.end())
					continue;

				const double baseTime = baseIt->second;
				double predicted = baseTime;

				bool skip = false;

				for (const auto& item : variant.subset(variant::Type::Independent)) {
					const Degree* degree = item.getDegree();
					const variant::Value value = variant.get(hash, item);

					// determine support
					const variant::Hash supportHash = baseHash + variant.hash(item, value);
					const auto supportIt = times.find(supportHash);

					if (hash == supportHash || supportIt == times.end()) {
						skip = true;
						break;
					}

					const double supportTime = supportIt->second;
					predicted += supportTime - baseTime;
				}

				if(skip)
					continue;

				overall.first += predicted;
				overall.second++;
			}
		});

		printf("\n");
	}

	Pair time = overallThreads.combine([](Pair x, Pair y) {
		return std::make_pair(x.first + y.first, x.second + y.second);
	});

	time.first /= 1000.0; // in s
	while(prof.step()) {
		time.first += prof.get<double>(0);
	}

	L_INFO("Time: %.10fs // %llu", time.first, time.second + profCnt);
}

//-------------------------------------------------------------------
void Misc::predict(void) {
	FILE* file = fopen("predicted.txt", "w");
	fprintf(file, "Call\tKernel\tHash\tMeasured\tOld\tNew\tType\n");
	
	Stmt call("SELECT call_id, kernel_id FROM calls;");
	Stmt run ("SELECT hash, duration FROM runs WHERE call_id = ?");
	Stmt cnt ("SELECT COUNT(*) FROM runs WHERE call_id = ?");
	
	// for all calls
	std::unordered_map<variant::Hash, double> times;
	
	while(call.step()) {
		const uint32_t callId = call.get<uint32_t>(0);
		const Kernel& kernel = Static::getKernel(call.get<uint32_t>(1));
		const Variant& variant = kernel.getVariant();

		run.reset();
		cnt.reset();
		
		run.bind(0, callId);
		cnt.bind(0, callId);
		
		cnt.step();
		const uint32_t runCnt = cnt.get<uint32_t>(0);
			
		times.clear();
		times.reserve(runCnt);
		
		// store times
		while(run.step()) {
			const variant::Hash hash = run.get<variant::Hash>(0);
			const double duration = run.get<uint64_t>(1) / 1000000.0;
			times.emplace(MAP_EMPLACE(hash, duration));
		}
		
		const double zeroBaseTime = times.at(0);
		
		std::set<variant::Hash> baseConfigs;
		std::set<variant::Hash> supportConfigs;
		
		//bool isSupport = false;
		
		// iterate times
		for(const auto& it : times) {
			const variant::Hash hash = it.first;
			const double duration = it.second;
			
			const variant::Hash baseHash = variant.convert(hash, variant::Type::Shared);
			baseConfigs.emplace(baseHash);
			const double domainBaseTime = times.at(baseHash);
			
			double oldPredTime = domainBaseTime;
			double newPredTime = domainBaseTime;
			
			for(const auto& item : variant.subset(variant::Type::Independent)) {
				const Degree* degree = item.getDegree();
				const variant::Value value = variant.get(hash, item);
				
				const bool isNew = degree->isGlobal() || degree->getType() == degree::Type::ArrayMemory;
				
				// determine base
				const double oldBaseTime = domainBaseTime;
				const double newBaseTime = isNew ? zeroBaseTime : domainBaseTime;
							
				// determine support
				const variant::Hash supportHash = variant.hash(item, value);
				supportConfigs.emplace(baseHash + supportHash);
				const double oldSupportTime = times.at(baseHash + supportHash);
				const double newSupportTime = 
					times.at((isNew ? 0 : baseHash) + supportHash)
				;
				
				// determine delta
				const double oldDelta = oldSupportTime - oldBaseTime;
				const double newDelta = newSupportTime - newBaseTime;
				
				// accumulate
				oldPredTime += oldDelta;
				newPredTime += newDelta;
			}
			
			const char* type = "P";
			
			if(baseConfigs.find(hash) != baseConfigs.end())
				type = "B";
			else if(supportConfigs.find(hash) != supportConfigs.end())
				type = "S";
			
			fprintf(file, "%u\t%s\t%u\t%f\t%f\t%f\t%s\n", callId, kernel.getName(), (uint32_t)hash, duration, oldPredTime, newPredTime, type);
		}
	}
	
	fclose(file);
}

//-------------------------------------------------------------------
void Misc::initDB(int argc, char** argv) {
	// print usage
	if(argc > 2) {
		L_INFO("usage: matog-misc [db-file]");
		exit(0);
	}

	// set db
	if(argc == 2)
		DB::setDBFile(argv[1]);

	// init Static
	Static::init();

	// init Options
	Options::init();
}

//-------------------------------------------------------------------
	}
}