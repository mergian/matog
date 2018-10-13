// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_UTIL_MEM_H
#define __MATOG_UTIL_MEM_H

#include "dll.h"
#include <cstdint>

#ifndef NDEBUG
#include <map>
#include <mutex>
#endif

namespace matog {
	namespace util {
//-------------------------------------------------------------------
/// Helper class to trace mem leaks
class MATOG_UTIL_DLL Mem {
#ifndef NDEBUG
private:
	/// Allocation items
	class MATOG_UTIL_DLL MemItem {
	public:
		const uint64_t	size;		///< element size
		const uint64_t	count;		///< count of elements
		const char*		file;		///< file where allocated
		const int		line;		///< line where allocated

		/// default constructor
		MemItem(void);
		
		/// create MemItem with size, count, file and line
		MemItem(const uint64_t size, const uint64_t count, const char* file, const int line);

		/// copy constructor
		MemItem(const MemItem& item);
	};

	/// Pair typedef for map
	typedef std::pair<const void*, MemItem> MapPair_t;

	/// Map that stores allocations
	class MATOG_UTIL_DLL Map : public std::map<const void*, MemItem> {};

	Map			m_map;		///< stores allocations
	uint64_t	m_peak;		///< stores peak memory consumption
	uint64_t	m_size;		///< stores current memory consumption
	uint32_t	m_error;	///< counts occuring alloc/free errors
	std::mutex	m_mutex;

	/// print error if ptr is already registered
	void printAlreadyAdded	(const void* ptr, const char* file, const int line, Map::const_iterator it);

public:
	/// init mem tracer
	Mem (void);

	/// check if mem leak is present
	~Mem(void);

	/// insert ptr into mem tracer
	template<class T>
	T* insert(T* ptr, const uint64_t count, const char* file, const int line) {
		insert((void*)ptr, count, sizeof(T), file, line);
		return ptr;
	}

	/// insert ptr into mem tracer
	void insert(void* ptr, const uint64_t count, const size_t size, const char* file, const int line);

	/// free ptr from mem tracer
	void freePtr(const void* ptr, const bool isArray, const char* file, const int line);

	/// print report
	void print(void);

	/// get instance of mem tracer
	static Mem& getInstance(void);
#endif
	
public:
	/// creates a copy of the given string an registers it
	static const char* copyString(const char*);
};
//-------------------------------------------------------------------
	}
}

//-------------------------------------------------------------------
#ifndef NDEBUG
/// usage: NEW(Class(...))
#define NEW(...)			matog::util::Mem::getInstance().insert(new __VA_ARGS__, 0, __FILE__, __LINE__)
/// usage: NEW_P(new ...) or NEW_P(malloc(...))
#define NEW_P(ptr)			matog::util::Mem::getInstance().insert(ptr, 0, __FILE__, __LINE__)
/// usage: NEW_A(Class, 15)
#define NEW_A(type, cnt)	matog::util::Mem::getInstance().insert(new type[(size_t)cnt], cnt, __FILE__, __LINE__)
/// usage: NEW_AP(new Class[15], 15)
#define NEW_AP(ptr, cnt)	matog::util::Mem::getInstance().insert(ptr, cnt, __FILE__, __LINE__)

/// usage: FREE(ptr) use only if created with NEW or NEW_P
template<typename T>
void FREE(T* ptr) {
	matog::util::Mem::getInstance().freePtr(ptr, false, __FILE__, __LINE__); 
	delete (ptr);
}

/// usage: FREE_A(ptr) use only if created with NEW_A or NEW_AP
template<typename T>
void FREE_A(T* ptr) {
	matog::util::Mem::getInstance().freePtr(ptr, true, __FILE__, __LINE__); 
	delete[] (ptr);
}
#else
/// usage: NEW(Class(...))
#define NEW(...)			new __VA_ARGS__
/// usage: NEW_P(new ...) or NEW_P(malloc(...))
#define NEW_P(ptr)			ptr
/// usage: NEW_A(Class, 15)
#define NEW_A(type, cnt)	new type[(size_t)cnt]
/// usage: NEW_AP(new Class[15], 15)
#define NEW_AP(ptr, cnt)	ptr
/// usage: FREE(ptr) use only if created with NEW or NEW_P
#define FREE(ptr)			delete (ptr)
/// usage: FREE_A(ptr) use only if created with NEW_A or NEW_AP
#define FREE_A(ptr)			delete[] (ptr)
#endif

/// usage: NEW_STRING(src)
#define NEW_STRING(src) matog::util::Mem::copyString(src)

//-------------------------------------------------------------------
#endif