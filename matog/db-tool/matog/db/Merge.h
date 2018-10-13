// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DB_MERGE_H
#define __MATOG_DB_MERGE_H

#include <list>
#include <matog/shared/DB.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
class Merge {
	typedef std::map<uint32_t, uint32_t> IdMap;
	IdMap m_profilingIds;
	IdMap m_moduleIds;
	IdMap m_arrayIds;
	IdMap m_kernelIds;
	IdMap m_gpuIds;
	IdMap m_callIds;
	IdMap m_runIds;
	IdMap m_copyIds;
	IdMap m_arrayInstanceIds;
	IdMap m_arrayInstanceTypeIds;

	void check					(void);
		
	void merge					(void);
	void mergeProfilings		(void);
	void mergeConfigs			(void);
	void mergeModules			(void);
	void mergeModuleDegrees		(void);
	void mergeKernels			(void);
	void mergeKernelDegrees		(void);
	void mergeCubins			(void);
	void mergeCubinMeta			(void);
	void mergeGPUs				(void);
	void mergeCalls				(void);
	void mergeCallMeta			(void);
	void mergeRuns				(void);
	void mergeEvents			(void);
	void mergeCopies			(void);
	void mergeArrayInstanceTypes(void);
	void mergeArrayInstances	(void);
	void mergeArrayInstanceMeta	(void);
	void mergeMetricsLookup		(void);

public:
	Merge(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif