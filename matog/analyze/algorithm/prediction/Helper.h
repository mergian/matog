// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYSE_ALGORITHM_PREDICTION_HELPER
#define __MATOG_ANALYSE_ALGORITHM_PREDICTION_HELPER

#include <matog/analyze.h>
#include <set>
#include <map>
#include <list>

namespace matog {
	namespace analyze {
		namespace algorithm {
			namespace prediction {
//-------------------------------------------------------------------
class Helper {
// static -----------------------------------------------------------
	typedef std::map<const Kernel*, variant::List> Domains;
	static Domains s_domains;

public:
	static void									initDomains		(void);
	static void									getDomains		(variant::List& domains, const graph::Call* call);
	static std::pair<variant::Hash, double>		findBestHash	(const variant::Hash domain, const graph::Call* call);
	static std::pair<variant::Value, double>	findBestValue	(const variant::Hash domain, const graph::Call* call, const variant::Item& item);
	static double								predict			(const variant::Hash hash, const graph::Call* call);
};

//-------------------------------------------------------------------
			}
		}
	}
}

#endif