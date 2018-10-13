// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_LOG_RESULT_H
#define __MATOG_LOG_RESULT_H

#include <stdint.h>
#include "dll.h"
#include <cuda.h>
#define WIN32_LEAN_AND_MEAN 1 // fix for the inconsistent definition of this macro...
#include <cupti.h>

/// defines the Sqlite return type (int) as an own type.
typedef int sqliteResult;

namespace matog {
	namespace log {
//-------------------------------------------------------------------
/// Generic Result Object, that can process CUDA, CUPTI, MATOG and SQLite results
class MATOG_LOG_DLL Result {
private:
	int			m_errorNumber;	///< Error Number
	const char*	m_error;		///< Error Name
	const char* m_message;		///< Error Message
	int			m_line;			///< occuring line
	const char*	m_file;			///< occuring file

	/// get error name for CUptiResult
	static const char* getCUPTIErrorString(const CUptiResult error);

public:
	/// unknown error
	Result(void);

	/// CUDA
	Result(CUresult cuda);

	/// CUPTI
	Result(CUptiResult cupti);

	/// SQLite with db reference
	Result(sqliteResult sql, void* db);

	/// String Error
	Result(const char* error);

	/// String Error, with explicit error message
	Result(const char* error, const char* message);

	/// Is result successful?
	inline bool			isSuccess	(void) const						{ return m_error == 0; }

	/// Get __FILE__
	inline const char*	getFile		(void) const						{ return m_file; }

	/// Get __LINE__
	inline int			getLine		(void) const						{ return m_line; }

	/// Sets __FILE__, __LINE__
	inline Result&		setFL		(const char* file, const int line)	{ m_file = file; m_line = line; return *this; }

	/// Print Message
	void				print		(void) const;
};

//-------------------------------------------------------------------
	}
}

#endif