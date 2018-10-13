// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_OCCUPANCY
#define __MATOG_OCCUPANCY

#include <cstdint>
#include <matog/internal.h>
#include <matog/enums.h>

namespace matog {
	namespace cuda {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL Occupancy {
private:
	static uint32_t divUp(const uint32_t A, const uint32_t B);
	static uint32_t divDown(const uint32_t A, const uint32_t B);

	uint32_t ComputeCapability;
	uint32_t ThreadCount;

	uint32_t Threads_Warp;
	uint32_t Warps_Multiprocessor;
	uint32_t Threads_Multiprocessor;
	uint32_t ThreadsBlocks_Multiprocessor;
	uint32_t RegisterFileSize_Multiprocessor;
	uint32_t MaxRegisters_Block;
	uint32_t RegisterAllocationUnitSize;
	uint32_t MaxRegisters_Thread;
	uint32_t SharedMemoryAllocationUnitSize;
	uint32_t WarpAllocationGranularity;
	uint32_t MaxWarpThreadBlockSize;

	uint32_t getSharedMemoryBytes(const L1Cache cache) const;

public:
	Occupancy(const uint32_t _ThreadCount, const GPU& gpu);
	double calc(const uint32_t registers, const uint32_t smem, const L1Cache cache) const;
	uint32_t calcLimit(const uint32_t registers, const uint32_t smem, const L1Cache cache, const double maxOccupancy) const;
};
//-------------------------------------------------------------------
	}
}

#endif