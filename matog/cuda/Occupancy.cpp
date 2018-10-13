// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/cuda/Occupancy.h>
#include <matog/GPU.h>
#include <matog/log.h>

namespace matog {
	namespace cuda {
//-------------------------------------------------------------------
Occupancy::Occupancy(const uint32_t _ThreadCount, const GPU& gpu) : ComputeCapability(gpu.getCC()), ThreadCount(_ThreadCount) {
	if(ComputeCapability == 0 || ThreadCount == 0) {
		L_DEBUG("Unable to calculate occupancy: unknown GPU or unknown thread count");
		return;
	}

	// set gpu data
	switch(ComputeCapability) {
	case 20:
	case 21:
		Threads_Warp						= 32;
		Warps_Multiprocessor				= 48;
		Threads_Multiprocessor				= 1536;
		ThreadsBlocks_Multiprocessor		= 8;
		RegisterFileSize_Multiprocessor		= 32768;
		MaxRegisters_Block					= 32768;
		RegisterAllocationUnitSize			= 64;
		MaxRegisters_Thread					= 63;
		SharedMemoryAllocationUnitSize		= 128;
		WarpAllocationGranularity			= 2;
		MaxWarpThreadBlockSize				= 1024;
		break;

	case 30:
		Threads_Warp						= 32;
		Warps_Multiprocessor				= 64;
		Threads_Multiprocessor				= 2048;
		ThreadsBlocks_Multiprocessor		= 16;
		RegisterFileSize_Multiprocessor		= 65536;
		MaxRegisters_Block					= 65536;
		RegisterAllocationUnitSize			= 256;
		MaxRegisters_Thread					= 63;
		SharedMemoryAllocationUnitSize		= 256;
		WarpAllocationGranularity			= 4;
		MaxWarpThreadBlockSize				= 1024;
		break;

	case 32:
		Threads_Warp						= 32;
		Warps_Multiprocessor				= 64;
		Threads_Multiprocessor				= 2048;
		ThreadsBlocks_Multiprocessor		= 16;
		RegisterFileSize_Multiprocessor		= 65536;
		MaxRegisters_Block					= 65536;
		RegisterAllocationUnitSize			= 256;
		MaxRegisters_Thread					= 255;
		SharedMemoryAllocationUnitSize		= 256;
		WarpAllocationGranularity			= 4;
		MaxWarpThreadBlockSize				= 1024;
		break;

	case 35:
		Threads_Warp						= 32;
		Warps_Multiprocessor				= 64;
		Threads_Multiprocessor				= 2048;
		ThreadsBlocks_Multiprocessor		= 16;
		RegisterFileSize_Multiprocessor		= 65536;
		MaxRegisters_Block					= 65536;
		RegisterAllocationUnitSize			= 256;
		MaxRegisters_Thread					= 255;
		SharedMemoryAllocationUnitSize		= 256;
		WarpAllocationGranularity			= 4;
		MaxWarpThreadBlockSize				= 1024;
		break;

	case 37:
		Threads_Warp						= 32;
		Warps_Multiprocessor				= 64;
		Threads_Multiprocessor				= 2048;
		ThreadsBlocks_Multiprocessor		= 16;
		RegisterFileSize_Multiprocessor		= 131072;
		MaxRegisters_Block					= 65536;
		RegisterAllocationUnitSize			= 256;
		MaxRegisters_Thread					= 255;
		SharedMemoryAllocationUnitSize		= 256;
		WarpAllocationGranularity			= 4;
		MaxWarpThreadBlockSize				= 1024;
		break;

	case 50:
	case 52:
		Threads_Warp						= 32;
		Warps_Multiprocessor				= 64;
		Threads_Multiprocessor				= 2048;
		ThreadsBlocks_Multiprocessor		= 32;
		RegisterFileSize_Multiprocessor		= 65536;
		MaxRegisters_Block					= 65536;
		RegisterAllocationUnitSize			= 256;
		MaxRegisters_Thread					= 255;
		SharedMemoryAllocationUnitSize		= 256;
		WarpAllocationGranularity			= 4;
		MaxWarpThreadBlockSize				= 1024;
		break;
		
	case 53:
		Threads_Warp						= 32;
		Warps_Multiprocessor				= 64;
		Threads_Multiprocessor				= 2048;
		ThreadsBlocks_Multiprocessor		= 32;
		RegisterFileSize_Multiprocessor		= 65536;
		MaxRegisters_Block					= 32768;
		RegisterAllocationUnitSize			= 256;
		MaxRegisters_Thread					= 255;
		SharedMemoryAllocationUnitSize		= 256;
		WarpAllocationGranularity			= 4;
		MaxWarpThreadBlockSize				= 1024;
		break;
		
	case 60:
	case 61:
		Threads_Warp						= 32;
		Warps_Multiprocessor				= 64;
		Threads_Multiprocessor				= 2048;
		ThreadsBlocks_Multiprocessor		= 32;
		RegisterFileSize_Multiprocessor		= 65536;
		MaxRegisters_Block					= 65536;
		RegisterAllocationUnitSize			= 256;
		MaxRegisters_Thread					= 255;
		SharedMemoryAllocationUnitSize		= 256;
		WarpAllocationGranularity			= 4;
		MaxWarpThreadBlockSize				= 1024;
		break;

	default:
		THROW("UNKNOWN_GPU_ARCHITECTURE");
	}
}

//-------------------------------------------------------------------
uint32_t Occupancy::getSharedMemoryBytes(const L1Cache cache) const {
	switch(ComputeCapability) {
	case 20:
	case 21:
		if(cache == L1Cache::SM) return 49152;
		return 16384;
	
	case 30:
	case 32:
	case 35:
		if(cache == L1Cache::SM) return 49152;
		if(cache == L1Cache::EQ) return 32768;
		return 16384;

	case 37:
		if(cache == L1Cache::SM) return 114688;
		if(cache == L1Cache::EQ) return 98304;
		return 81920;

	case 50:
		return 65536;

	case 52:
		return 98304;
		
	case 53:
	case 60:
	case 61:
		return 65536;

	default:
		break;
	}

	THROW("UNKNOWN_GPU_ARCHITECTURE");
	return 0;
}

//-------------------------------------------------------------------
uint32_t Occupancy::divUp(const uint32_t A, const uint32_t B) {
	const uint32_t mod = A % B;
	
	if(mod == 0)
		return A;

	return A - mod + B;
}

//-------------------------------------------------------------------
uint32_t Occupancy::divDown(const uint32_t A, const uint32_t B) {
	const uint32_t mod = A % B;
	
	if(mod == 0)
		return A;

	return A - mod;
}

//-------------------------------------------------------------------
double Occupancy::calc(const uint32_t registers, const uint32_t smem, const L1Cache cache) const {
	if (ComputeCapability == 0 || ThreadCount == 0)
		return 0.0;

	double occupancy = 0.0;

	// calc warp limits
	uint32_t WarpsPerBlock = ThreadCount / Threads_Warp;
	if((ThreadCount % Threads_Warp) != 0)
		WarpsPerBlock += 1;

	uint32_t BlocksPerSM = Warps_Multiprocessor / WarpsPerBlock;

	if(BlocksPerSM > ThreadsBlocks_Multiprocessor)
		BlocksPerSM = ThreadsBlocks_Multiprocessor;

	// calc register limits
	THROWIF(registers > MaxRegisters_Thread || registers == 0, "WRONG_REGISTER_COUNT");

	uint32_t RegistersPerSM			= divDown(MaxRegisters_Block /divUp(Threads_Warp * registers, RegisterAllocationUnitSize), WarpAllocationGranularity);
	uint32_t RegisterBlocksPerSM	= (RegistersPerSM / WarpsPerBlock) * (RegisterFileSize_Multiprocessor / MaxRegisters_Block);

	// calc shared mem limits
	uint32_t UsedSharedMemory = divUp(smem, SharedMemoryAllocationUnitSize);
	uint32_t SMBlocksPerSM = UsedSharedMemory == 0 ? BlocksPerSM : getSharedMemoryBytes(cache) / UsedSharedMemory;

	// find limiting factor
	uint32_t minBlocks = BlocksPerSM;

	if(minBlocks > RegisterBlocksPerSM)	minBlocks = RegisterBlocksPerSM;
	if(minBlocks > SMBlocksPerSM)		minBlocks = SMBlocksPerSM;

	// calc occupancy
	occupancy = (minBlocks * WarpsPerBlock) / (double)Warps_Multiprocessor;

	return occupancy;
}

//-------------------------------------------------------------------
uint32_t Occupancy::calcLimit(const uint32_t registers, const uint32_t smem, const L1Cache cache, const double maxOccupancy) const {
	// calc warp limits
	uint32_t WarpsPerBlock = ThreadCount / Threads_Warp;

	// calc shared mem limits
	uint32_t UsedSharedMemory = divUp(smem, SharedMemoryAllocationUnitSize);

	// calc occupancy;
	uint32_t limitBytes = (uint32_t)((WarpsPerBlock * getSharedMemoryBytes(cache)) / (maxOccupancy * Warps_Multiprocessor));

	if(UsedSharedMemory > limitBytes)
		limitBytes = 0;
	else
		limitBytes -= UsedSharedMemory;

	return limitBytes;
}

//-------------------------------------------------------------------
	}
}