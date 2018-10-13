// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYSE_ALGORITHM_PREDICTION_PREDICTION
#define __MATOG_ANALYSE_ALGORITHM_PREDICTION_PREDICTION

#include <matog/analyze.h>
#include <set>
#include <map>
#include <list>

namespace matog {
	namespace analyze {
		namespace algorithm {
			namespace prediction {
//-------------------------------------------------------------------
class Prediction {
private:
	typedef std::map<graph::Call*, std::map<variant::Hash, std::pair<variant::Hash, double> > > Local;

	const graph::Graph&		m_graph;
	Local					m_local;

	void initLocal		(void);
	void processGlobal	(void);
	void processLocal	(void);

public:
	Prediction(const graph::Graph& graph);
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif