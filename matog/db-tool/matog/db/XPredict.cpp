// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/shared.h>
#include <matog/log.h>
#include <matog/util.h>
#include <matog/cuda.h>
#include "../db.h"
#include <cstdio>
#include <cfloat>
#include <limits>
#include <cmath>
#include <algorithm>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
#ifdef WIN32
#pragma warning (disable: 4996)
#endif

//-------------------------------------------------------------------
XPredict::XPredict(int argc, char** argv) {
	// init
	CHECK(Static::init());

	// execute
	DB& db = DB::getInstance();
	
	sqlite3_stmt* stmt;

	if(argc == 0) {
		CHECK(db.prepare(stmt, "SELECT id, gpu_id, kernel_id, user_hash FROM " DB_CALLS " ORDER BY " DB_CALLS ".id;"));
		CHECK(db.exec(stmt, SQL_BIND(XPredict::execute, this)));
	} else {
		CHECK(db.prepare(stmt, "SELECT id, gpu_id, kernel_id, user_hash FROM " DB_CALLS " WHERE " DB_CALLS ".id = ?;"));
		
		for(int i = 0; i < argc; i++) {
			uint32_t callId = atoi(argv[i]);
			CHECK(sqlite3_bind_int(stmt, 1, callId));
			CHECK(db.step(stmt, SQL_BIND(XPredict::execute, this)));
			CHECK(sqlite3_reset(stmt));
		}

		CHECK(db.finalize(stmt));
	}
}

//-------------------------------------------------------------------
SQL_FUNCTION(XPredict::execute) {
	assert(column_count == 4);
	Item(result, sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2), sqlite3_column_int(stmt, 3));
}

//-------------------------------------------------------------------
XPredict::Item::Item(Result& result, const uint32_t callId, const uint32_t gpuId, const uint32_t kernelId, const uint32_t userHash) :
	m_current		(&m_iterationBuffer[0]),
	m_previous		(&m_iterationBuffer[1]),
	m_db			(DB::getInstance()),
	m_gpu			(Static::getGPU(gpuId)),
	m_kernel		(Static::getKernel(kernelId)),
	m_module		(Static::getModule(m_kernel.getModuleId())),
	m_degrees		(m_module.getDegrees()),
	m_callId		(callId),
	m_userHash		(userHash),
	m_lastDegree	(0)
{
	L_INFO("Processing Call %u // %s::%s", callId, m_module.getName().c_str(), m_kernel.getName().c_str());

	m_shared.insert		(m_module.getSharedDegrees().begin(), m_module.getSharedDegrees().end());
	m_nonShared.insert	(m_module.getNonSharedDegrees().begin(), m_module.getNonSharedDegrees().end());
	//m_nonShared.insert	(m_variants.getDegrees().begin(), m_variants.getDegrees().end());

	// remove uninteresting degrees with only one variant
	for(auto it = m_shared.begin(); it != m_shared.end();) {
		if((*it)->getValueCount(&m_gpu) == 1)
			it = m_shared.erase(it);
		else
			it++;
	}

	for(auto it = m_nonShared.begin(); it != m_nonShared.end();) {
		if((*it)->getValueCount(&m_gpu) == 1)
			it = m_nonShared.erase(it);
		else
			it++;
	}

	// Profiling Results
	result = profilingResults();
	if(!result.isSuccess())
		return;

	// exec
	result = exec();
}

//-------------------------------------------------------------------
void XPredict::Item::Iteration::clear(void) {
	m_times.clear();
	m_bestTimes.clear();
	m_baselineTime.clear();
	m_degreeVariance.clear();
	m_predicted.clear();
}

//-------------------------------------------------------------------
Result XPredict::Item::swapBuffers(void) {
	auto tmp = m_current;
	m_current = m_previous;
	m_previous = tmp;
	m_current->clear();

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::open(FILE*& file, const char* name, const char* ext, const bool appendNumber) {
	std::ostringstream ss;
	ss << m_callId << "--" << m_module.getName().c_str() << "--" << m_kernel.getName().c_str()  << name;

	if(appendNumber)
		ss << "_" << m_shared.size();

	ss << "." << ext;

	std::string tmp = ss.str();
	
	// open file
	#if WIN32
	if(fopen_s(&file, tmp.c_str(), "w")) {
		RCHECK(MatogResult::IO_EXCEPTION);
	}
	#else
	file = fopen(tmp.c_str(), "w");
	if(m_file == NULL) {
		L_ERROR("ERROR: %u --> %s", errno, strerror(errno));
		RCHECK(MatogResult::IO_EXCEPTION);
	}
	#endif

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::close(FILE*& file) {
	if(fclose(file))
		RCHECK(MatogResult::IO_EXCEPTION);
	file = 0;
	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::profilingResults(void) { // print all profiling data
	FILE* file;
	sqlite3_stmt* stmt;

	// get profiling results
	RCHECK(open(file, "_profiling", "txt", false));

	RCHECK(m_db.prepare(stmt, "SELECT hash, duration, value_flt FROM " DB_RUNS ", " DB_METRICS " WHERE call_id = ? AND id = run_id AND metric_id = 1005 ORDER BY duration ASC;"));
	RCHECK(sqlite3_bind_int(stmt, 1, m_callId));

	uint32_t row = 1;
	SQL_LOOP(stmt)
		const uint32_t hash = sqlite3_column_int(stmt, 0);
		const double time	= sqlite3_column_int64(stmt, 1)/1000000.0;
		const double ipc	= sqlite3_column_double(stmt, 2);
		fprintf(file, "%u\t%f\t%f\n", row, time, ipc);
		m_ipc.emplace				(MAP_EMPLACE(hash, ipc));
		m_profilingTimes.emplace	(MAP_EMPLACE(hash, time));
		m_row.emplace				(MAP_EMPLACE(hash, row++));
	SQL_LEND()

	RCHECK(m_db.finalize(stmt));
	RCHECK(close(file));

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::baselineResults(void) {
	Pairs pairs;

	// init baselines
	auto it = m_degrees.begin();
	for(uint32_t i = 0; i < m_degrees.size(); i++, it++)
		m_config[i] = (*it)->getBaseline();

	bool isFirst = true;
	
	RCHECK(open(m_file, "_baseline", "txt"));

	// iterate all degrees
	for(const Degree* degree : m_nonShared) {
		const uint32_t index = m_module.getDegreeIndex(degree);

		// init variance
		std::pair<double, double>& minMax = m_current->m_degreeVariance[degree];
		minMax.first  = DBL_MAX;
		minMax.second = -DBL_MAX;

		// init
		const std::vector<MatogConfig>& items = degree->getValues(&m_gpu);

		// iterate all
		MatogConfig old = m_config[index];

		for(const MatogConfig c : items) {
			m_config[index] = c;
			RCHECK(recursive(degree, m_shared.begin(), pairs));
			
			if(isFirst) {
				isFirst = false;

				for(auto& it : m_current->m_times) {
					m_current->m_baselineTime[VHash::calcCustomHash(it.first, m_shared, m_degrees)] = it.second;
				}

				RCHECK(close(m_file));
				RCHECK(open(m_file, "_baseclass", "txt"));
			}
		}

		m_config[index] = old;
	}

	RCHECK(close(m_file));

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::gnuPlot(void) {
	std::ostringstream ss;
	ss << m_callId << "--" << m_module.getName().c_str() << "--" << m_kernel.getName().c_str();

	std::string name = ss.str();

	FILE* file;
	
	/*RCHECK(open(file, "", "gnu_pdf"));
	fprintf(file, "set terminal pdf enhanced\n");
	fprintf(file, "set output \"%s_%u.pdf\"\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "set key left top\n\n");
	fprintf(file, "set grid xtics lw 0.5 \n");
	fprintf(file, "set grid ytics lw 0.5 \n");
	fprintf(file, "set title \"%s (%u)  %s\" \n", name.c_str(), (int)m_shared.size(), m_lastDegree == 0 ? "" : m_lastDegree->getName().c_str());
	fprintf(file, "set label 1 \"Mean: %f \\nS^2: %f \\nS: %f\\nS%%: %f%%\\nMin: %f\\nMax: %f\" at graph 0.1, graph 0.9\n", m_current->m_mean, m_current->m_variance, m_current->m_stdabw, m_current->m_stdabwp, m_current->m_min, m_current->m_max);
	fprintf(file, "plot \"%s_profiling.txt\" using 1:2 ti \"Measured\"  w p pt 7 lc rgb \"#000000\" ps 0.05 lw 1, \\\n", name.c_str());
	fprintf(file, "     \"%s_pred_sorted_%u.txt\" using 1:2 ti \"Predicted Function\"  w p pt 7 lc rgb \"#8A2BE2\" ps 0.05 lw 1, \\\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "     \"%s_baseclass_%u.txt\" using 1:2 ti \"Baseclass\" w p pt 2 lc rgb \"#0000FF\" ps 0.5 lw 1, \\\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "     \"%s_baseline_%u.txt\"  using 1:2 ti \"Baseline\"  w p pt 2 lc rgb \"#DC143C\" ps 0.5 lw 1, \\\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "     \"%s_predicted_%u.txt\" using 1:2 ti \"Predicted\" w p pt 6 lc rgb \"#00CC00\" ps 0.05 lw 1, \\\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "     \"%s_best_%u.txt\"      using 1:2 ti \"Best\"      w p pt 6 lc rgb \"#00FFFF\" ps 1 lw 2\n", name.c_str(), (int)m_shared.size());
	RCHECK(close(file));*/

	RCHECK(open(file, "", "gnu_png"));
	fprintf(file, "set terminal pngcairo size 1920,1080 enhanced\n");
	fprintf(file, "set output \"%s_%u.png\"\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "set key left top\n\n");
	fprintf(file, "set grid xtics lw 0.5 \n");
	fprintf(file, "set grid ytics lw 0.5 \n");
	fprintf(file, "set title \"%s (%u)  %s\" \n", name.c_str(), (int)m_shared.size(), m_lastDegree == 0 ? "" : m_lastDegree->getName().c_str());
	fprintf(file, "set label 1 \"Mean: %f \\nS^2: %f \\nS: %f\\nS%%: %f%%\\nMin: %f\\nMax: %f\" at graph 0.1, graph 0.9\n", m_current->m_mean, m_current->m_variance, m_current->m_stdabw, m_current->m_stdabwp, m_current->m_min, m_current->m_max);
	fprintf(file, "plot \"%s_profiling.txt\"        using 1:2 ti \"Measured\"            w p pt 6 lc rgb \"#000000\" ps 0.25 lw 1, \\\n", name.c_str());
	fprintf(file, "     \"%s_predicted_%u.txt\"     using 1:2 ti \"Predicted\"           w p pt 6 lc rgb \"#00CC00\" ps 0.25 lw 1, \\\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "     \"%s_pred_sorted_%u.txt\"   using 1:2 ti \"Predicted Function\"  w p pt 6 lc rgb \"#8A2BE2\" ps 0.25 lw 1, \\\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "     \"%s_baseclass_%u.txt\"     using 1:2 ti \"Baseclass\"           w p pt 2 lc rgb \"#0000FF\" ps 1.5 lw 1, \\\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "     \"%s_baseline_%u.txt\"      using 1:2 ti \"Baseline\"            w p pt 2 lc rgb \"#DC143C\" ps 1.5 lw 1, \\\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "     \"%s_best_%u.txt\"          using 1:2 ti \"Best\"                w p pt 6 lc rgb \"#00FFFF\" ps 2.5 lw 2, \\\n", name.c_str(), (int)m_shared.size());
	fprintf(file, "     \"%s_best2_sampled_%u.txt\" using 1:2 ti \"Best2 Samples\"       w p pt 6 lc rgb \"#FFFF00\" ps 2.5 lw 2\n", name.c_str(), (int)m_shared.size());
	RCHECK(close(file));
	
	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::selectNextDegree(void) {
	const Degree* nextDegree	= 0;
	double diff					= 0.0;

	for(auto& it : m_current->m_degreeVariance) {
		double d = it.second.second - it.second.first;
		if(d > diff) {
			nextDegree = it.first;
			diff = d;
		}
	}

	CHECKIF(nextDegree == 0, MatogResult::UNKNOWN_ERROR);

	m_shared.emplace(nextDegree);
	m_nonShared.erase(nextDegree);

	m_lastDegree = nextDegree;

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::exec(void) {
	// iterate until there are no more degrees nonShared
	//while(!m_nonShared.empty()) {
		RCHECK(baselineResults());
		RCHECK(predictedResults());
		RCHECK(predictedBest());
		RCHECK(calcStats());
		RCHECK(gnuPlot());

		// process next iteration
		//RCHECK(selectNextDegree());
		//RCHECK(swapBuffers());
	//}

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::calcStats(void) {
	m_current->m_variance	= 0.0;
	m_current->m_mean		= 0.0;
	m_current->m_stdabw		= 0.0;
	m_current->m_min		= 0.0;
	m_current->m_max		= 0.0;
	m_current->m_stdabwp	= 0;

	if(m_current->m_predicted.empty())
		return MatogResult::SUCCESS;

	m_current->m_min		= DBL_MAX;
	m_current->m_max		= -DBL_MAX;

	std::vector<double> diffs;
	diffs.reserve(m_current->m_predicted.size());

	// calc mean
	for(auto& it : m_current->m_predicted) {
		auto pit = m_profilingTimes.find(it.first);

		if(pit == m_profilingTimes.end())
			continue;

		double diff = pit->second - it.second;
		m_current->m_mean += diff;

		diffs.emplace_back(diff);

		if(m_current->m_min > diff)
			m_current->m_min = diff;

		if(m_current->m_max < diff)
			m_current->m_max = diff;
	}

	m_current->m_mean /= (double)diffs.size();

	// calc variance
	for(const double diff : diffs) {
		double tmp = diff - m_current->m_mean;
		m_current->m_variance += tmp * tmp;
	}

	m_current->m_variance /= (double)diffs.size();
	m_current->m_stdabw = sqrt(m_current->m_variance);
	m_current->m_stdabwp = m_current->m_stdabw / (m_current->m_max - m_current->m_min);

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::predictedBest2Recursive(FILE* file, shared::Degree::Set::const_iterator it, Pairs& pairs, uint32_t& bestHash, double& bestTime) {
	// is this the end of the list?
	if(it == m_shared.end()) {
		uint32_t hash = VHash::calcHash(m_config, m_degrees);
		
		auto it = m_profilingTimes.find(hash);

		if(it != m_profilingTimes.end()) {
			double time = it->second;

			fprintf(file, "%u\t%f\n", m_row[hash], time);

			if(time < bestTime) {
				bestTime = time;
				bestHash = hash;
			}
		}

		return MatogResult::SUCCESS;
	} 
	// iterate all remaining items
	else {
		// get current degree
		Degree::Set::const_iterator pit = it;

		// get next iterator
		it++;

		// iterate all
		const Degree* degree = *pit;

		// get variants for this degree
		const std::vector<MatogConfig>& items = degree->getValues();

		// iterate
		const uint32_t index = m_module.getDegreeIndex(degree);

		for(const auto& item : items) {
			m_config[index] = item;

			pairs.emplace_back(std::make_pair(degree, item));
			predictedBest2Recursive(file, it, pairs, bestHash, bestTime);
			pairs.pop_back();
		}
	}

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::predictedBest(void) {
	FILE* file;
	RCHECK(open(file, "_best", "txt"));

	std::unordered_map<uint32_t, double> totalTime;

	for(auto& it : m_current->m_bestTimes) {
		double& time = totalTime[it.first];
		double bt = m_current->m_baselineTime[it.first];
		time = bt;

		for(auto& sit : it.second) {
			time += sit.second.first - bt;
		}
	}

	auto best = totalTime.end();
	double time = DBL_MAX;

	for(auto it = totalTime.begin(); it != totalTime.end(); it++) {
		if(it->second < time) {
			time = it->second;
			best = it;
		}
	}

	// iterate all measured points and check if we are better than BEST
	auto measuredBest = m_current->m_times.end();

	for(auto it = m_current->m_times.begin(); it != m_current->m_times.end(); it++) {
		if(it->second < time) {
			measuredBest = it;
			time = it->second;
		}
	}

	if(best != totalTime.end()) {
		bool isFirst = true;

		auto& map = m_current->m_bestTimes[best->first];
		
		// init all not in map
		for(const Degree* d : m_nonShared) {
			if(map.find(d) == map.end())
				m_config[m_module.getDegreeIndex(d)] = d->getBaseline();
		}

		// iterate all in map
		for(auto& it : map) {
			const Degree* degree = it.first;

			BestItem& bestItem = it.second;
			MatogConfig conf[MATOG_MAX_DEGREES_PER_TARGET];
			m_module.getConfig(conf, bestItem.second);

			// apply shared config if is first
			if(isFirst) {
				isFirst = false;

				for(const Degree* d : m_shared) {
					const uint32_t index = m_module.getDegreeIndex(d);
					m_config[index] = conf[index];
				}
			}

			// apply non shared
			const uint32_t index = m_module.getDegreeIndex(degree);
			m_config[index] = conf[index];
		}

		uint32_t bestHash = VHash::calcHash(m_config, m_degrees);
		fprintf(file, "%u\t%f\n", m_row[bestHash], m_profilingTimes[bestHash]);
		fprintf(file, "%u\t%f\n", m_row[bestHash], time);
	}

	RCHECK(close(file));

	// init baseline for shared
	for(const Degree* d : m_shared) {
		m_config[m_module.getDegreeIndex(d)] = d->getBaseline();
	}

	Pairs pairs;
	uint32_t bestHash;
	double bestTime = DBL_MAX;
	RCHECK(open(file, "_best2_sampled", "txt"));
	RCHECK(predictedBest2Recursive(file, m_shared.begin(), pairs, bestHash, bestTime));
	RCHECK(close(file));

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::predictedResults(void) {
	FILE* file;
	RCHECK(open(file, "_predicted", "txt"));

	// get all results and calculate the predicted value for these
	for(const auto& it : m_profilingTimes) {
		const uint32_t hash = it.first;

		// only if not in times map
		if(m_current->m_times.find(hash) == m_current->m_times.end()) {
			MatogConfig current[MATOG_MAX_DEGREES_PER_TARGET];
			m_module.getConfig(current, hash);

			// set correct shared degree values
			for(const Degree* shared : m_shared) {
				const uint32_t index = m_module.getDegreeIndex(shared);
				m_config[index] = current[index];
			}

			// init baseline time
			uint32_t baselineHash = VHash::calcHash(m_config, m_degrees);

			const auto baselineIt = m_current->m_times.find(baselineHash);
			CHECKIF(baselineIt == m_current->m_times.end(), MatogResult::UNKNOWN_ERROR);
			const double baselineTime = baselineIt->second;
			//const double baselineIpc = m_ipc.find(baselineHash.getHash())->second;

			double predicted = baselineTime;

			// iterate degrees
			for(const Degree* degree : m_nonShared) {
				const uint32_t index = m_module.getDegreeIndex(degree);

				// iterate all
				MatogConfig old = m_config[index];
			
				m_config[index] = current[index];
				uint32_t tmpHash = VHash::calcHash(m_config, m_degrees);

				// skip as the result will be 0 anyway
				if(tmpHash != baselineHash) {
					const auto it = m_current->m_times.find(tmpHash);
					CHECKIF(it == m_current->m_times.end(), MatogResult::UNKNOWN_ERROR);
					//const double ipc = m_ipc.find(tmpHash.getHash())->second;
					//predicted += (it->second * ipc / baselineIpc) - baselineTime;
					predicted += it->second - baselineTime;
				}

				// reset
				m_config[index] = old;
			}

			//predicted *= baselineIpc / m_ipc.find(vhash.getHash())->second;

			m_current->m_predicted[hash] = predicted;
			fprintf(file, "%u\t%f\n", m_row[hash], predicted);
		}
	}

	RCHECK(close(file));

	RCHECK(open(file, "_pred_sorted", "txt"));
	std::vector<double> vector;
	vector.resize(m_current->m_predicted.size() + m_current->m_times.size());

	uint32_t i = 0;
	for(const auto& it : m_current->m_predicted) {
		vector[i++] = it.second;
	}

	for(const auto& it : m_current->m_times) {
		vector[i++] = it.second;
	}

	std::sort(vector.begin(), vector.end());
	for(uint32_t i = 0; i < vector.size(); i++) {
		fprintf(file, "%u\t%f\n", i + 1, vector[i]);
	}

	RCHECK(close(file));

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
Result XPredict::Item::recursive(const Degree* current, Degree::Set::const_iterator it, Pairs& pairs) {
	// is this the end of the list?
	if(it == m_shared.end()) {
		uint32_t hash = VHash::calcHash(m_config, m_degrees);
		
		auto it = m_profilingTimes.find(hash);
		if(it == m_profilingTimes.end())
			return MatogResult::SUCCESS;

		uint32_t sharedHash = VHash::calcCustomHash(hash, m_shared, m_degrees);

		// insert best time for shared Hash
		auto& map = m_current->m_bestTimes[sharedHash];
		auto bestIt = map.find(current);

		if(bestIt == map.end()) {
			bestIt = map.emplace(MAP_EMPLACE(current, std::make_pair(it->second, hash))).first;
		} else {
			if(bestIt->second.first > it->second) {
				bestIt->second.first  = it->second;
				bestIt->second.second = hash;
			}
		}

		// insert best for degree
		auto dit = m_current->m_bestDegree.find(current);
		if(dit == m_current->m_bestDegree.end()) {
			m_current->m_bestDegree.emplace(MAP_EMPLACE(current, std::make_pair(it->second, hash)));
		} else {
			if(it->second < dit->second.first)
				dit->second.first = it->second;
		}

		// set min/max for degree
		std::pair<double, double>& minMax = m_current->m_degreeVariance[current];
		if(minMax.first > it->second)
			minMax.first = it->second;

		if(minMax.second < it->second)
			minMax.second = it->second;

		// only print if not already in m_times
		if(m_current->m_times.find(hash) == m_current->m_times.end()) {
			m_current->m_times[hash] = it->second;
			fprintf(m_file, "%u\t%f\n", m_row[hash], it->second);
		}

		return MatogResult::SUCCESS;
	} 
	// iterate all remaining items
	else {
		// get current degree
		Degree::Set::const_iterator pit = it;

		// get next iterator
		it++;

		// iterate all
		const Degree* degree = *pit;

		// get variants for this degree
		const std::vector<MatogConfig>& items =	degree->getValues();

		// iterate
		const uint32_t index = m_module.getDegreeIndex(degree);

		for(const auto& item : items) {
			m_config[index] = item;

			pairs.emplace_back(std::make_pair(degree, item));
			recursive(current, it, pairs);
			pairs.pop_back();
		}
	}

	return MatogResult::SUCCESS;
}

//-------------------------------------------------------------------
	}
}