// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DB_PREDICT_H
#define __MATOG_DB_PREDICT_H

#include <list>
#include <map>
#include <matog/shared/Kernel.h>
#include <matog/shared/Module.h>
#include <matog/shared/Degree.h>
#include <matog/shared/Config.h>
#include <matog/shared/DB.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
class Predict {
private:
	class Item {
	private:
		struct Run {
			double duration;
			double clock;
			double occupancy;

			inline Run(const double _duration, const double _clock, const double _occupancy) :
				duration(_duration), clock(_clock / 1000.0), occupancy(_occupancy) {}
		};

		const shared::Kernel&	m_kernel;
		const shared::Module&	m_module;
		uint32_t				m_callId;
		
		typedef std::list< std::pair<const shared::Degree*, uint32_t> > SharedDegrees;

		std::map<shared::Config::Hash, Run> m_times;

		void getTimes	(void);
		void predict	(void);

		const Run& getRun(const uint64_t hash);

	public:
		Item(const uint32_t callId, const uint32_t kernelId);
	};

public:
	Predict(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif