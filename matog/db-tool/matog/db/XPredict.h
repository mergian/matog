// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DB_XPREDICT_H
#define __MATOG_DB_XPREDICT_H

#include <list>
#include <map>
#include <matog/shared/Kernel.h>
#include <matog/shared/Module.h>
#include <matog/shared/Degree.h>
#include <matog/shared/DB.h>
#include <matog/log/Result.h>
#include <matog/util/SQLiteDB.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
class XPredict {
private:
	typedef std::list<std::pair<const shared::Degree*, MatogConfig> > Pairs;

	class Item {
	private:
		typedef std::pair<double, uint32_t>								BestItem;
		typedef std::unordered_map<const shared::Degree*,  BestItem>	BestDegree;

		MatogConfig											m_config[MATOG_MAX_DEGREES_PER_TARGET];
		FILE*												m_file;

		// fixed for all iterations
		std::unordered_map<uint32_t, double>				m_ipc;
		std::unordered_map<uint32_t, double>				m_profilingTimes;
		std::unordered_map<uint32_t, uint32_t>				m_row;

		// new for each iteration
		struct Iteration {
			std::unordered_map<uint32_t, double>				m_times;
			std::unordered_map<uint32_t, BestDegree>			m_bestTimes;
			std::unordered_map<uint32_t, double>				m_baselineTime;
			std::unordered_map<uint32_t, double>				m_predicted;
			BestDegree											m_bestDegree;
			std::unordered_map<const shared::Degree*, std::pair<double, double> >	m_degreeVariance;
			double												m_mean, m_variance, m_stdabw, m_min, m_max, m_stdabwp;

			void clear(void);
		};

		Iteration							m_iterationBuffer[2];
		Iteration*							m_current;
		Iteration*							m_previous;

		shared::DB&							m_db;
		const shared::GPU&					m_gpu;
		const shared::Kernel&				m_kernel;
		const shared::Module&				m_module;
		const shared::Degree::Set&			m_degrees;
		shared::Degree::Set					m_shared;
		shared::Degree::Set					m_nonShared;
		const uint32_t						m_callId;
		uint32_t							m_userHash;
		const shared::Degree*				m_lastDegree;

		log::Result recursive				(const shared::Degree* current, shared::Degree::Set::const_iterator it, Pairs& pairs);
		log::Result exec					(void);
		log::Result profilingResults		(void);
		log::Result baselineResults			(void);
		log::Result predictedResults		(void);
		log::Result predictedBest			(void);
		log::Result predictedBest2Recursive	(FILE* file, shared::Degree::Set::const_iterator it, Pairs& pairs, uint32_t& bestHash, double& bestTime);
		log::Result gnuPlot					(void);
		log::Result selectNextDegree		(void);
		log::Result swapBuffers				(void);
		log::Result calcStats				(void);
		log::Result open					(FILE*& file, const char* name, const char* ext, const bool appendNumber = true);
		log::Result close					(FILE*& file);

	public:
		Item(log::Result& result, const uint32_t callId, const uint32_t gpuId, const uint32_t kernelId, const uint32_t userHash);
	};

	SQL_FUNCTION(execute);
	
public:
	XPredict(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif