// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_UTIL_SQLSTMT
#define __MATOG_UTIL_SQLSTMT

#include <matog/util.h>
#include <sqlite3/sqlite3.h>
#include <matog/util/Mem.h>

namespace matog {
	namespace util {
/// Forward declaration
class MATOG_UTIL_DLL SQLiteDB;

//-------------------------------------------------------------------
/// Stmt class for SQLiteDB
class MATOG_UTIL_DLL SQLStmt {
private:
	SQLiteDB&		m_db;
	sqlite3_stmt*	m_stmt;
	const char*		m_query;

	
public:
	/// open stmt
						SQLStmt			(const char* query, SQLiteDB& db);

	/// create object, but do not open stmt
						SQLStmt			(SQLiteDB& db);

	/// destructor
						~SQLStmt		(void);

	/// reuses the statement for another query
			SQLStmt&	operator()		(const char* query);

	/// resets query
			void		reset			(void);

	/// executes SQL statement (returns true if another row is available)
			bool		step			(void);

	/// executes SQL statement and returns rowId as U32
			uint32_t	step32			(void);

	/// executes SQL statement and returns rowId as U64
			uint64_t	step64			(void);

	/// binds value to position
			template<typename T>	SQLStmt& bind		(const uint32_t index, const T& value) { return bind(index, (int)value); }

	/// binds all cols of a stmt to this stmt
			SQLStmt&	bind		(const SQLStmt& other);

	/// gets value at position
			template<typename T> T	get					(const uint32_t index) const { return (T)get<int>(index);	}
		
	/// returns the size of a field
			uint32_t getSize(const uint32_t index) const;

	/// returns the query
			inline const char*	getQuery(void) const {
				return m_query;
			}

	/// returns the column type
			inline int			getColumnType(const uint32_t index) const {
				return sqlite3_column_type(m_stmt, index);
			}

	/// returns the column count
			inline int			getColumnCount(void) const {
				return sqlite3_column_count(m_stmt);
			}

	/// returns the parameter count
			inline int			getParameterCount(void) const {
				return sqlite3_bind_parameter_count(m_stmt);
			}
};

//-------------------------------------------------------------------
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<int>					(const uint32_t index, const int& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<bool>					(const uint32_t index, const bool& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<uint64_t>				(const uint32_t index, const uint64_t& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<int64_t>				(const uint32_t index, const int64_t& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<const char*>			(const uint32_t index, const char * const& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<char*>					(const uint32_t index, char * const& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<const unsigned char*>	(const uint32_t index, const unsigned char * const& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<unsigned char*>		(const uint32_t index, unsigned char * const& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<float>					(const uint32_t index, const float& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<double>				(const uint32_t index, const double& value);
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<blob>					(const uint32_t index, const blob& value);

//-------------------------------------------------------------------
template<> MATOG_UTIL_DLL int			SQLStmt::get<int>				(const uint32_t index) const;
template<> MATOG_UTIL_DLL bool			SQLStmt::get<bool>				(const uint32_t index) const;
template<> MATOG_UTIL_DLL uint64_t		SQLStmt::get<uint64_t>			(const uint32_t index) const;
template<> MATOG_UTIL_DLL int64_t		SQLStmt::get<int64_t>			(const uint32_t index) const;
template<> MATOG_UTIL_DLL const char*	SQLStmt::get<const char*>		(const uint32_t index) const;
template<> MATOG_UTIL_DLL float			SQLStmt::get<float>				(const uint32_t index) const;
template<> MATOG_UTIL_DLL double		SQLStmt::get<double>			(const uint32_t index) const;
template<> MATOG_UTIL_DLL blob			SQLStmt::get<blob>				(const uint32_t index) const;

//-------------------------------------------------------------------
	}
}

#endif