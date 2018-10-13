// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/util/String.h>
#include <matog/util/Mem.h>
#include <matog/util/Hash.h>
#include <matog/log.h>
#include <algorithm>
#include <iomanip>
#include <functional> 
#include <cctype>
#include <sstream>
#include <string>
#include <matog/macros.h>

namespace matog {
	namespace util {
//-------------------------------------------------------------------
void String::init(void) {
	// make sure that init is only called at the beginning
	assert(m_ptr == 0 && m_size == 0 && m_length == 0);
	
	// init everything
	m_ptr				= m_local;
	m_size				= sizeof(m_local);
	m_length			= 0;
	m_local[m_length]	= 0;
}

//-------------------------------------------------------------------
void String::setLength(const uint32_t length) {
	// get old size
	const uint32_t oldSize = m_size;
	
	// set new length
	m_length = length;

	// calculate new required size
	uint32_t newSize = m_length + 1;
	newSize = newSize + 16 - (newSize % 16);

	// do we need more space?
	if(newSize > oldSize) {
		// get old ptr
		char* oldPtr = m_ptr;
		
		// allocate new ptr
		m_ptr = NEW_A(char, newSize);
		m_size = newSize;

		// copy content from old to new
		memcpy(m_ptr, oldPtr, oldSize);

		// free ptr if necessary
		if(oldPtr != m_local) {
			assert(oldPtr);
			FREE_A(oldPtr);
		}
	}

	// set ending 0
	m_ptr[m_length] = 0;

	assert(m_length < m_size);
}

//-------------------------------------------------------------------
String& String::append(const String& str) {
	const uint32_t oldLen = getLength();
	const uint32_t newLen = getLength() + str.getLength();
	
	setLength(newLen);
	memcpy(m_ptr + oldLen, str.m_ptr, str.getLength());

	return *this;
}

//-------------------------------------------------------------------
String& String::append(const char c) {
	const uint32_t newLen = getLength() + 1;
	
	setLength(newLen);
	m_ptr[newLen] = c;

	return *this;
}

//-------------------------------------------------------------------
String& String::append(const unsigned char c) {
	return append((char)c);
}

//-------------------------------------------------------------------
void String::prune(void) {
	if(m_size > (m_length + 1)) {
		m_size = m_length + 1;

		char* newPtr = NEW_A(char, m_size);
		memcpy(newPtr, m_ptr, m_size);

		FREE_A(m_ptr);
		m_ptr = newPtr;
	}
}

//-------------------------------------------------------------------
void String::clear(void) {
	m_length = 0;
	m_ptr[0] = 0;
}

//-------------------------------------------------------------------
void String::free(void) {
	if(m_ptr != m_local) {
		FREE_A(m_ptr);
		m_ptr	= m_local;
		m_size	= sizeof(m_local);
	}
	
	clear();
}

//-------------------------------------------------------------------
void String::copy(const char* str, const uint32_t length) {
	setLength(length);
	
	// copy only if str is != 0
	if(str && length)
		memcpy(m_ptr, str, length);
}

//-------------------------------------------------------------------
String::String(void) :
	m_ptr	(0),
	m_length(0),
	m_size	(0)
{
	init();
}

//-------------------------------------------------------------------
String::String(const char* str) :
	m_ptr	(0),
	m_length(0),
	m_size	(0)
{
	init();
	if(str)
		copy(str, (uint32_t)strlen(str));
}

//-------------------------------------------------------------------
String::String(const char c) :
	m_ptr	(0),
	m_length(0),
	m_size	(0)
{
	init();
	setLength(1);
	m_ptr[0] = c;
	m_ptr[1] = 0;
}

//-------------------------------------------------------------------
String::String(const unsigned char* str) :
	m_ptr	(0),
	m_length(0),
	m_size	(0)
{
	init();
	if(str)
		copy((const char*)str, (uint32_t)strlen((const char*)str));
}

//-------------------------------------------------------------------
String::String(const unsigned char c) :
	m_ptr	(0),
	m_length(0),
	m_size	(0)
{
	init();
	setLength(1);
	m_ptr[0] = c;
	m_ptr[1] = 0;
}

//-------------------------------------------------------------------
String::String(const String& str):
	m_ptr	(0),
	m_length(0),
	m_size	(0)
{
	init();
	copy(str.m_ptr, str.m_length);
}

//-------------------------------------------------------------------
String::~String(void) {
	if(m_ptr != 0 && m_ptr != m_local) {
		FREE_A(m_ptr);
	}
}

//-------------------------------------------------------------------
bool String::operator<(const String& str) const {
	uint32_t min = getLength();
	
	if(str.getLength() < min)
		min = str.getLength();

	for(uint32_t i = 0; i < min; i++) {
		if(m_ptr[i] < str.m_ptr[i])
			return true;
		else if(m_ptr[i] > str.m_ptr[i])
			return false;
	}

	return getLength() < str.getLength();
}

//-------------------------------------------------------------------
bool String::operator==(const String& str) const {
	if(getLength() != str.getLength())
		return false;

	for(uint32_t i = 0; i < getLength(); i++) {
		if(m_ptr[i] != str.m_ptr[i])
			return false;
	}

	return true;
}

//-------------------------------------------------------------------
bool String::operator!=(const String& str) const {
	return !(*this == str);
}

//-------------------------------------------------------------------
String& String::operator=(const String& str) {
	copy(str, str.getLength());
	return *this;
}

//-------------------------------------------------------------------
String& String::operator=(const char* str) {
	if(str)
		copy(str, (uint32_t)strlen(str));
	else
		this->clear();

	return *this;
}

//-------------------------------------------------------------------
String& String::operator=(const unsigned char* str) {
	return *this = (const char*)str;
}

//-------------------------------------------------------------------
String String::substr(const uint32_t start, uint32_t end) const {
	assert(start < getLength());

	if(end == UINT_MAX)
		end = getLength();

	const uint32_t len = end - start;

	String str;
	str.copy(m_ptr + start, len);

	return str;
}

//-------------------------------------------------------------------
String& String::replaceAll(const String& what, const String& with) {
	std::string s(m_ptr);

	for(size_t start_pos = s.find(what.c_str()); start_pos != SIZE_MAX; start_pos = s.find(what.c_str(), start_pos)){
		s.replace(start_pos, what.getLength(), with.c_str());
	}

	copy(s.c_str(), (uint32_t)s.size());
	return *this;
}

//-------------------------------------------------------------------
bool String::startsWith(const String& str, const bool ignoreCase) const {
	if(str.getLength() > getLength())
		return false;

	// do not ignore case
	if(!ignoreCase) {
		for(uint32_t i = 0; i < str.getLength(); i++) {
			if(m_ptr[i] != str.m_ptr[i])
				return false;
		}
	}
	// ignore case
	else {
		String a(*this);
		String b(str);
		a.toLowerCase();
		b.toLowerCase();

		for(uint32_t i = 0; i < b.getLength(); i++) {
			if(a.m_ptr[i] != b.m_ptr[i])
				return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------
bool String::endsWith(const String& str, const bool ignoreCase) const {
	if(str.getLength() > getLength())
		return false;

	uint32_t offset = getLength() - str.getLength() - 1;

	// do not ignore case
	if(!ignoreCase) {
		for(uint32_t i = 0; i < str.getLength(); i++) {
			if(m_ptr[offset + i] != str.m_ptr[i])
				return false;
		}
	}
	// ignore case
	else {
		String a(*this);
		String b(str);
		a.toLowerCase();
		b.toLowerCase();

		for(uint32_t i = 0; i < b.getLength(); i++) {
			if(a.m_ptr[offset + i] != b.m_ptr[i])
				return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------
uint32_t String::find(const String& str, const uint32_t offset) const {
	if(str.getLength() > getLength())
		return UINT_MAX;

	bool allFailed = true;
	uint32_t firstO = UINT_MAX;

	for(uint32_t i = offset; i < (getLength() - str.getLength() + 1); i++) {
		bool failed = false;

		for(uint32_t n = 0; n < str.getLength(); n++) {
			if(m_ptr[i + n] != str.m_ptr[n]) {
				failed = true;
				break;
			}
		}

		allFailed = allFailed && failed;

		if (!failed && firstO == UINT_MAX)
			firstO = i;
	}

	if (!allFailed)
		return firstO;

	return UINT_MAX;
}

//-------------------------------------------------------------------
uint32_t String::find(const char c, const uint32_t offset) const {
	for(uint32_t i = offset; i < getLength(); i++) {
		if(m_ptr[i] == c)
			return i;
	}

	return UINT_MAX;
}

//-------------------------------------------------------------------
void String::explode(std::list<String>& list, const char separator) const {
	std::string str(c_str());
	std::istringstream ss(str);

	std::string line;

	while(ss.good()) {
		std::getline(ss, line, separator);

		if(!line.empty())
			list.push_back(line);
	}
}

//-------------------------------------------------------------------
String&	String::appendSlash(void) {
	if(getLength() > 0) {
		if(m_ptr[getLength() - 1] == '/')
			return *this;
	}

	return append('/');
}

//-------------------------------------------------------------------
String& String::toLowerCase(void) {
	for(uint32_t i = 0; i < m_length; i++) {
		if(m_ptr[i] >= 'A' && m_ptr[i] <= 'Z')
			m_ptr[i] += 'a' - 'A';
	}

	return *this;
}

//-------------------------------------------------------------------
String& String::toUpperCase(void) {
	for(uint32_t i = 0; i < m_length; i++) {
		if(m_ptr[i] >= 'a' && m_ptr[i] <= 'z')
			m_ptr[i] += 'A' - 'a';
	}

	return *this;
}

//-------------------------------------------------------------------
String& String::toCamelCase(void) {
	if(m_length > 0 && m_ptr[0] >= 'a' && m_ptr[0] <= 'z')
		m_ptr[0] += 'A' - 'a';

	return *this;
}

//-------------------------------------------------------------------
String String::toHexString(const uint64_t value, const uint32_t width) {
	std::ostringstream ss;
	ss << std::uppercase << std::hex << std::setfill('0') << std::setw(width) << value;
	return ss.str();
}

//-------------------------------------------------------------------
String& String::ltrim(void) {
	std::string s(m_ptr);
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	copy(s.c_str(), (uint32_t)s.size());
	return *this;
}

//-------------------------------------------------------------------
String& String::rtrim(void) {
	std::string s(m_ptr);
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	copy(s.c_str(), (uint32_t)s.size());
	return *this;
}

//-------------------------------------------------------------------
String& String::trim(void) {
	ltrim();
	rtrim();
	return *this;
}

//-----------------------------------------------------------------
uint32_t String::count(const char c) const {
	uint32_t cnt = 0;

	for(uint32_t i = 0; i < getLength(); i++) {
		if(m_ptr[i] == c)
			cnt++;
	}

	return cnt;
}

//-----------------------------------------------------------------
String&	String::removeIf(bool (*charsToRemove)(char)) {
	std::string s(c_str());
	s.erase(std::remove_if(s.begin(), s.end(), charsToRemove), s.end());

	copy(s.c_str(), (uint32_t)s.size());
	return *this;
}

//-----------------------------------------------------------------
void String::print(void) const {
	L_DEBUG("[String / len=%u / size=%u / local=%s / str=%s]", m_length, m_size, (m_ptr == m_local ? "true" : "false"), m_ptr);
}

//-----------------------------------------------------------------
uint32_t String::crc32(void) const { 
	return Hash::crc32(m_ptr); 
}

//-----------------------------------------------------------------
	}
}