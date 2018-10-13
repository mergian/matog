// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_INTERNAL
#define __MATOG_INTERNAL

#include <matog/dll.h>
#include <cstdint>
#include <vector_types.h>
#define WIN32_LEAN_AND_MEAN 1 // fix for the inconsistent definition of this macro...
#include <cupti.h>
#include <list>

namespace matog {
	// private interface
	class Array;
	class DB;
	class Degree;
	class GPU;
	class Kernel;
	class Module;
	class Occupancy;
	class Options;
	class Static;
	class Variant;
		
	namespace array {
		class Set;
		class Field;
		struct SetComparator;
		enum class Type;

		namespace field {
			enum class Type;
			enum class Memory;
			extern MATOG_INTERNAL_DLL Type getTypeByName(const char* name);
			extern MATOG_INTERNAL_DLL uint32_t getTypeSize(const Type type);
			extern MATOG_INTERNAL_DLL const char* getTypeString(const Type type);
			extern MATOG_INTERNAL_DLL const char* getTexTypeString(const uint32_t size);
			extern MATOG_INTERNAL_DLL const char* getTexTypeString(const Type type);
			struct VType;
		}
	}
		
	namespace db {
		struct Stmt;
		class Clear;
		class PredictionModel;
	}
		
	namespace degree {
		class Set;
		struct SetComparator;
		enum class Type;	
		class PrettyPrinter;				
	}
		
	namespace kernel {
		class Meta;
		class Code;

		namespace hint {
			class Identifier;
			class RWHints;
			class CountHints;
			class RefHints;
		}
	}
		
	namespace variant {
		class Item;
		class Set;
		class Items;
		class Iterator;
		class Index;
		class Mul;
		enum class Type;
					
		typedef uint8_t Value;
		typedef uint32_t CompressedHash;
		typedef uint64_t Hash;
		typedef std::list<Hash> List;
	}

	namespace event {
		enum class Type;
		enum class ProfilingAlgorithm;
		
		extern MATOG_INTERNAL_DLL void		dbDeleteAll					(void);
		extern MATOG_INTERNAL_DLL uint32_t	dbInsertProfiling			(const event::ProfilingAlgorithm profiler);
		extern MATOG_INTERNAL_DLL void		dbDeleteAllProfilings		(void);
		extern MATOG_INTERNAL_DLL void		dbUpdateProfilingDuration	(const uint32_t profilingId);
		extern MATOG_INTERNAL_DLL uint32_t	dbInsertCall				(const uint32_t profilingId, const uint32_t decisionId, const uint32_t gpuId, const uint32_t kernelId, const uint32_t userHash, const uint3 blocks, const uint3 threads, const uint32_t dynamicSharedMem);
		extern MATOG_INTERNAL_DLL void		dbDeleteAllCalls			(void);
		extern MATOG_INTERNAL_DLL uint32_t	dbInsertRun					(const uint32_t callId, const uint64_t hash);
		extern MATOG_INTERNAL_DLL void		dbDeleteAllRuns				(void);
		extern MATOG_INTERNAL_DLL void		dbDeleteRun					(const uint32_t runId);
		extern MATOG_INTERNAL_DLL void		dbSetRunData				(const uint32_t runId, const uint64_t duration, const uint32_t requested, const uint32_t executed);
		extern MATOG_INTERNAL_DLL void		dbSetRunSharedMemory		(const uint32_t runId, const uint32_t externMem, const uint32_t dynamicMem);
		extern MATOG_INTERNAL_DLL void		dbSetRunOccupancy			(const uint32_t runId, const double occupancy);
		extern MATOG_INTERNAL_DLL void		dbInsertMetric				(const uint32_t runId, const CUpti_MetricID metricId, CUpti_MetricValue value, CUpti_MetricValueKind kind);
		extern MATOG_INTERNAL_DLL void		dbDeleteMetrics				(const uint32_t runId);
		extern MATOG_INTERNAL_DLL void		dbDeleteAllMetrics			(void);
		extern MATOG_INTERNAL_DLL uint32_t	dbInsertEvent				(const uint32_t profilingId, const uint32_t decisionId);
		extern MATOG_INTERNAL_DLL void		dbInsertEventData			(const uint32_t eventId, const uint32_t sourceEventId, const uint32_t paramId, const uint32_t decisionId, const uint32_t decisionOffset, const uint32_t dim, const float value);
		extern MATOG_INTERNAL_DLL uint32_t	dbInsertDecision			(const uint32_t hash, const uint32_t arrayId, const uint32_t kernelId, const uint32_t gpuId, const char* file, const int line);
		extern MATOG_INTERNAL_DLL void		dbDeleteAllEvents			(void);
		extern MATOG_INTERNAL_DLL uint32_t	dbSelectDecisionId			(const uint32_t hash, const uint32_t arrayId, const uint32_t kernelId, const uint32_t gpuId);
		extern MATOG_INTERNAL_DLL void		printSummary				(const uint32_t id);
	}

	// cuda
	namespace cuda {
		class CUBINStore;
		class CUDA;
		class Jit;
		class NVDevice;
		enum class Architecture;

		namespace jit {
			class Target;
			class Item;
		}
	}
}
		
#endif