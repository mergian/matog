// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_UTIL_STRING_H
#define __MATOG_UTIL_STRING_H

#include "dll.h"
#include <string>
#include <sstream>
#include <cstdint>
#include <list>
#include <cassert>
#include <climits>

namespace matog {
	namespace util {
//-------------------------------------------------------------------
/// String Class with DLL Interface
class MATOG_UTIL_DLL String {
private:
	char		m_local[32];		///< stores short strings
	char*		m_ptr;				///< stores strings longer than sizeof(m_local)
	uint32_t	m_length;			///< lenght of string
	uint32_t	m_size;				///< size of allocated memory

	/// init this object
	void		init					(void);

	/// copy string from str to this object
	void		copy					(const char* str, const uint32_t length);

	/// prune memory
	void		prune					(void);

	/// set length
	void		setLength				(const uint32_t length);

public:
				String					(void);
				String					(const char* str);
				String					(const char c);
				String					(const unsigned char* str);
				String					(const unsigned char c);
				String					(const String& str);
	inline		String					(const std::string& str) : m_ptr(0), m_length(0), m_size(0) { init(); copy(str.c_str(), (uint32_t)str.length()); }
	inline		String					(const std::ostringstream& stream) : m_ptr(0), m_length(0), m_size(0) { init(); const std::string str = stream.str(); copy(str.c_str(), (uint32_t)str.length()); }

	virtual		~String					(void);

	uint32_t	find					(const String& str, const uint32_t offset = 0) const;
	uint32_t	find					(const char c, const uint32_t offset = 0) const;
	
	bool		operator==				(const String& str) const;
	bool		operator!=				(const String& str) const;
	bool		operator<				(const String& str) const;
	String&		operator=				(const String& str);
	String&		operator=				(const char* str);
	String&		operator=				(const unsigned char* str);
	String		substr					(const uint32_t start, uint32_t end = UINT_MAX) const;
	String&		replaceAll				(const String& what, const String& with);
	String&		append					(const String& str);
	String&		append					(const char c);
	String&		append					(const unsigned char c);
	uint32_t	count					(const char c) const;
	bool		startsWith				(const String& str, const bool ignoreCase = true) const;
	bool		endsWith				(const String& str, const bool ignoreCase = true) const;
	void		explode					(std::list<String>& list, const char separator) const;
	void		clear					(void);
	void		free					(void);

	inline	uint32_t	getSize			(void) const					{ return m_size; }
	inline	uint32_t	getLength		(void) const					{ return m_length; }
	inline	bool		isEmpty			(void) const					{ return getLength() == 0; }

	inline	const char*	c_str			(void) const					{ assert(m_ptr); return m_ptr; }
	inline	operator const char*		(void) const					{ assert(m_ptr); return m_ptr; }
	inline	operator std::string		(void) const					{ assert(m_ptr); return std::string(m_ptr); }
	
	inline	String&		operator=		(const std::string& str) {
		copy(str.c_str(), (uint32_t)str.size());
		return *this;
	}

	inline	String&		operator=		(const std::ostringstream& stream) {
		const std::string str = stream.str();
		copy(str.c_str(), (uint32_t)str.size());
		return *this;
	}

	inline	bool		operator==		(const char* str) const			{ return *this == String(str); }
	inline	bool		operator==		(const std::string& str) const	{ return *this == String(str); }

	inline	uint64_t	asU64			(void) const					{ return strtoul(m_ptr, NULL, 0); }
	inline	int64_t		asS64			(void) const					{ return strtol(m_ptr, NULL, 0); }
	inline	uint32_t	asU32			(void) const					{ return (uint32_t)strtoul(m_ptr, NULL, 0); }
	inline	int32_t		asS32			(void) const					{ return (int32_t)atoi(m_ptr); }
	inline	uint16_t	asU16			(void) const					{ return (uint16_t)atoi(m_ptr); }
	inline	int16_t		asS16			(void) const					{ return (int16_t)atoi(m_ptr); }
	inline	uint8_t		asU8			(void) const					{ return (uint8_t)atoi(m_ptr); }
	inline	int8_t		asS8			(void) const					{ return (int8_t)atoi(m_ptr); }
	inline	bool		asBool			(void) const					{ return (startsWith("t") || asU32() != 0); }
	inline	double		asDouble		(void) const					{ return strtod(m_ptr, NULL);}
	inline	float		asFloat			(void) const					{ return (float)atof(m_ptr); }

			uint32_t	crc32			(void) const;

	template<typename T>
	static inline	String		toString(const T& value)				{ std::ostringstream ss; ss << value; return ss.str(); }
	static	String	toHexString			(const uint64_t value, const uint32_t width);

	static	void	pathToList			(const String& str, std::list<const char*>& list);
			String&	appendSlash			(void);

			String&	toLowerCase			(void);
			String&	toUpperCase			(void);
			String&	toCamelCase			(void);

			String&	ltrim				(void);
			String&	rtrim				(void);
			String&	trim				(void);
			String&	removeIf			(bool (*charsToRemove)(char));
			
			void	print				(void) const;
};

//-------------------------------------------------------------------
	}
}

#endif