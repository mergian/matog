// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/util/Mem.h>
#include <matog/util/Exit.h>
#include <matog/log.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <matog/macros.h>
#include <string>

namespace matog {
	namespace util {
#ifndef NDEBUG
//---------------------------------------------------------------------------
static Mem* mem_instance = 0;

//---------------------------------------------------------------------------
static void freeMem(void) {
	if(mem_instance)
		delete mem_instance;
}

//---------------------------------------------------------------------------
static std::string getSizeLabel(const uint64_t size) {
	std::stringstream out;	
	
	const char* unit[] = {"B", "kB", "MB", "GB"};

	float s = (float)size;
	unsigned char a = 0;
	
	while(s > 1024) {
		s /= 1024.0f;
		a++;
	}

	out << std::setprecision(2) << std::fixed << s << "" << unit[a];

	return out.str();
}

//-------------------------------------------------------------------
Mem& Mem::getInstance(void) {
	if(mem_instance == 0) {
		mem_instance = new Mem();
		Exit::add(freeMem);
	}
		
	return *mem_instance;
}

//-------------------------------------------------------------------
Mem::MemItem::MemItem(void) :
	size	(0),
	count	(0),
	file	(0),
	line	(0)
{
	assert(false);
}

//-------------------------------------------------------------------
Mem::MemItem::MemItem(const uint64_t size, const uint64_t count, const char* file, const int line) :
	size	(size),
	count	(count),
	file	(file),
	line	(line)
{}

//-------------------------------------------------------------------
Mem::MemItem::MemItem(const MemItem& item) :
	size	(item.size),
	count	(item.count),
	file	(item.file),
	line	(item.line) 
{}

//-------------------------------------------------------------------
Mem::Mem(void) : m_peak(0), m_size(0), m_error(0) {
}

//-------------------------------------------------------------------
Mem::~Mem(void) {
	print();
}

//-------------------------------------------------------------------
void Mem::insert(void* ptr, const uint64_t count, const size_t size, const char* file, const int line) {
	THROWIF(ptr == 0, "OUT_OF_MEMORY");
	
	std::lock_guard<std::mutex> lock(m_mutex);
	Map::iterator it = m_map.find(ptr);

	if(it == m_map.end()) {
		m_map.insert(MapPair_t(ptr, MemItem(size, count, file, line)));
		m_size += (count == 0 ? 1 : count) * size;

		if(m_size > m_peak)
			m_peak = m_size;
	} else {
		printAlreadyAdded((const void*)ptr, file, line, it);	
		m_error++;
	}
}

//-------------------------------------------------------------------
void Mem::freePtr(const void* ptr, const bool isArray, const char* file, const int line) {
	Map::iterator it;

	std::lock_guard<std::mutex> lock(m_mutex);
	if(ptr != 0 && (it = m_map.find(ptr)) != m_map.end()) {
		const uint64_t cnt = it->second.count;
		const uint64_t size = it->second.size * (cnt == 0 ? 1 : cnt);

		m_size -= size;
		m_map.erase(it);

		if(isArray && cnt == 0) {
			m_error++;
			L_WARN("ptr 0x%016llX is no array in %s (%u)", (unsigned long long)ptr, file, line);
		} else if(!isArray && cnt > 1) {
			m_error++;
			L_WARN("ptr 0x%016llX is an array in %s (%u)", (unsigned long long)ptr, file, line);
		}
	} else {
		m_error++;
		L_WARN("unable to remove ptr 0x%016llX in %s (%u)", (unsigned long long)ptr, file, line);
	}
}

//-------------------------------------------------------------------
void Mem::print(void) {
	L_DEBUG("Dynamic Memory Dump: %s used / %s peak. %u items left. %u errors occurred.", getSizeLabel(m_size).c_str(), getSizeLabel(m_peak).c_str(), (int)m_map.size(), m_error);

	uint32_t cnt = 0;

	for(auto&item : m_map) {
		L_WARN("0x%016llX %s in %s (%u)", (unsigned long long)item.first, getSizeLabel((item.second.count == 0 ? 1 : item.second.count) * item.second.size).c_str(), item.second.file, item.second.line);
		
		if(cnt++ == 30) {
			L_WARN("...");
			break;
		}
	}
}

//-------------------------------------------------------------------
void Mem::printAlreadyAdded(const void* ptr, const char* file, const int line, Map::const_iterator it) {
	L_WARN("pointer 0x%016llX added from %s (%u) was already added by %s (%u)", (unsigned long long)ptr, file, line, it->second.file, it->second.line);
}
#endif

//-------------------------------------------------------------------
const char* Mem::copyString(const char* src) {
	assert(src);
	const size_t len = strlen(src) + 1;
	char* out = NEW_A(char, len);
	memcpy(out, src, len);
	return out;
}

//-------------------------------------------------------------------
	}
}