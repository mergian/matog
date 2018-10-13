// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_RUNTIME_INTERN_CONTEXT
#define __MATOG_RUNTIME_INTERN_CONTEXT

#include <matog/cuda/CUDA.h>
#include <matog/runtime.h>
#include <matog/runtime/intern/DeviceArray.h>
#include <matog/runtime/intern/BaseArray.h>
#include <matog/runtime/intern/context/Cache.h>
#include <matog/runtime/intern/context/Meta.h>
#include <mutex>

namespace matog {
	namespace runtime {
		namespace intern {
			using context::Cache;
			using context::Meta;

//-------------------------------------------------------------------
class Context {
private:
	CUcontext									m_context;				///< CUcontext
	Cache										m_cache;				///< ModuleCache
	std::set<intern::BaseArray<>*>				m_arrays;				///< <array instances>
	std::map<uint64_t, intern::MemArray<>*>		m_decisions;			///< <decisionId | historyId, BaseArray*>
	std::map<uint32_t, uint32_t>				m_decisionOffsets;		///< <decisionId, maxOffsetId>
	std::map<uint32_t, std::string>				m_compiler;				///< <kernelid, compiler flags>
	std::map<CUdeviceptr, uint64_t>				m_allocations;			///< <CUdeviceptr, size>
	std::map<const void*, const Meta>			m_meta;					///< <value ptr, Meta>
	std::map<uint32_t, const Meta&>				m_metaById;				///< <decisionId, Meta&>

	static std::map<CUcontext, Context>			s_context;
	static std::mutex							s_mutex;

	uint32_t getCurrentOffsetId	(const uint32_t eventItemId) const;
	uint32_t getNewOffsetId		(const uint32_t eventItemId);

	inline uint64_t	getOffsetId(const uint32_t decisionId, const uint32_t offset) const {
		return (((uint64_t)offset) << 32) | decisionId;
	}

	inline uint64_t	getOffsetId(intern::MemArray<>* array) const {
		return getOffsetId(array->getDecisionId(), array->getOffsetId());
	}
	
public:
	inline Context(CUcontext context) : m_context(context), m_cache(context) {}
	Context(const Context&) = delete;

	static Context&		get					(CUcontext context = cuda::CUDA::getCurrentContext());
	static void			destroy				(CUcontext context = cuda::CUDA::getCurrentContext());

	float				getMetaValue		(const uint32_t decisionId, const uint32_t offset, const uint32_t dim);

	void				addMeta				(const void* ptr, const array::field::Type type, const char* file, const int line);
	void				removeMeta			(const void* ptr);
	
	void				addArray			(intern::BaseArray<>* array);
	void				removeArray			(intern::BaseArray<>* array);
	bool				isArray				(intern::BaseArray<>* array);

	void				addDecision			(intern::MemArray<>* array, const Kernel* kernel = 0, const GPU* gpu = 0);
	void				removeDecision		(intern::MemArray<>* array);
	void				copyDecision		(intern::MemArray<>* dest, intern::MemArray<>* src);

	void				addAllocation		(CUdeviceptr ptr, const uint64_t size);
	void				removeAllocation	(CUdeviceptr ptr);
	uint64_t			getAllocationSize	(CUdeviceptr ptr);

	Cache&				getCache			(void);
	
	void				setCompilerFlags	(const uint32_t kernelId, const char* flags);
	const char*			getCompilerFlags	(const uint32_t kernelId);

	CUcontext			getContext			(void);

	void				dbInsertCallMeta	(const uint32_t eventId, intern::DeviceArray* array);
	void				dbInsertArrayMeta	(const uint32_t eventId);
	void				dbInsertUserMeta	(const uint32_t eventId);
};

//-------------------------------------------------------------------
		}
	}
}
#endif