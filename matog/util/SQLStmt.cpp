// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <matog/util.h>
#include <sqlite3/sqlite3.h>
#include <matog/util/SQLStmt.h>
#include <matog/util/SQLiteDB.h>
#include <matog/util/Mem.h>
#include <matog/util/String.h>
#include <matog/macros.h>

using namespace matog::log;
using namespace matog::util;

namespace matog {
	namespace util {
//-------------------------------------------------------------------
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<int>					(const uint32_t index, const int& value)					{ CHECK(sqlite3_bind_int(m_stmt, index + 1, value), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<bool>					(const uint32_t index, const bool& value)					{ CHECK(sqlite3_bind_int(m_stmt, index + 1, value ? 1 : 0), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<uint64_t>				(const uint32_t index, const uint64_t& value)				{ CHECK(sqlite3_bind_int64(m_stmt, index + 1, value), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<int64_t>				(const uint32_t index, const int64_t& value)				{ CHECK(sqlite3_bind_int64(m_stmt, index + 1, value), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<const char*>			(const uint32_t index, const char * const& value)			{ CHECK(sqlite3_bind_text(m_stmt, index + 1, value, -1, SQLITE_STATIC), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<char*>					(const uint32_t index, char * const& value)					{ CHECK(sqlite3_bind_text(m_stmt, index + 1, value, -1, SQLITE_STATIC), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<const unsigned char*>(const uint32_t index, const unsigned char * const& value)	{ CHECK(sqlite3_bind_text(m_stmt, index + 1, (const char*)value, -1, SQLITE_STATIC), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<unsigned char*>		(const uint32_t index, unsigned char * const& value)		{ CHECK(sqlite3_bind_text(m_stmt, index + 1, (const char*)value, -1, SQLITE_STATIC), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<float>					(const uint32_t index, const float& value)					{ CHECK(sqlite3_bind_double(m_stmt, index + 1, value), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<double>				(const uint32_t index, const double& value)					{ CHECK(sqlite3_bind_double(m_stmt, index + 1, value), m_db); return *this; }
template<> MATOG_UTIL_DLL SQLStmt& SQLStmt::bind<blob>					(const uint32_t index, const blob& value)					{ 
	THROWIF(value.size >= UINT32_MAX, "we do not support to store so much data in the DB!")
	CHECK(sqlite3_bind_blob(m_stmt, index + 1, value.ptr, (uint32_t)value.size, SQLITE_STATIC), m_db); 
	return *this;
}

//-------------------------------------------------------------------
template<> MATOG_UTIL_DLL int			SQLStmt::get<int>			(const uint32_t index) const { return				sqlite3_column_int(m_stmt, index); }
template<> MATOG_UTIL_DLL bool			SQLStmt::get<bool>			(const uint32_t index) const { return				sqlite3_column_int(m_stmt, index) != 0; }
template<> MATOG_UTIL_DLL uint64_t		SQLStmt::get<uint64_t>		(const uint32_t index) const { return 				sqlite3_column_int64(m_stmt, index); }
template<> MATOG_UTIL_DLL int64_t		SQLStmt::get<int64_t>		(const uint32_t index) const { return 				sqlite3_column_int64(m_stmt, index); }
template<> MATOG_UTIL_DLL const char*	SQLStmt::get<const char*>	(const uint32_t index) const { return (const char*)	sqlite3_column_text(m_stmt, index); }
template<> MATOG_UTIL_DLL float			SQLStmt::get<float>			(const uint32_t index) const { return (float)		sqlite3_column_double(m_stmt, index); }
template<> MATOG_UTIL_DLL double		SQLStmt::get<double>		(const uint32_t index) const { return				sqlite3_column_double(m_stmt, index); }
template<> MATOG_UTIL_DLL blob			SQLStmt::get<blob>			(const uint32_t index) const { 
	return blob(sqlite3_column_blob(m_stmt, index), sqlite3_column_bytes(m_stmt, index));
}
template<> MATOG_UTIL_DLL const void*	SQLStmt::get<const void*>	(const uint32_t index) const { return sqlite3_column_blob(m_stmt, index); }

//-------------------------------------------------------------------
SQLStmt::SQLStmt(const char* query, SQLiteDB& db) : m_db(db), m_stmt(0), m_query(0) {
	(*this).operator()(query);
}

//-------------------------------------------------------------------
SQLStmt::SQLStmt(SQLiteDB& db) : m_db(db), m_stmt(0), m_query(0) {
}

//-------------------------------------------------------------------
SQLStmt& SQLStmt::operator()(const char* query) {
	L_INSANE("%s", query);

	m_query = query;

	if(m_stmt)
		CHECK(sqlite3_finalize(m_stmt), m_db);

	CHECK(sqlite3_prepare_v2(m_db, m_query, -1, &m_stmt, 0), m_db);

	return *this;
}

//-------------------------------------------------------------------
SQLStmt::~SQLStmt(void) {
	if(m_stmt)
		CHECK(sqlite3_finalize(m_stmt), m_db);
}

//-------------------------------------------------------------------
void SQLStmt::reset(void) {
	CHECK(sqlite3_reset(m_stmt), m_db);
}

//-------------------------------------------------------------------
bool SQLStmt::step() {
	sqliteResult result = sqlite3_step(m_stmt);
	CHECK(result, m_db);
	return result == SQLITE_ROW;
}

//-------------------------------------------------------------------
uint32_t SQLStmt::getSize(const uint32_t index) const {
	return sqlite3_column_bytes(m_stmt, index);
}

//-------------------------------------------------------------------
uint32_t SQLStmt::step32(void) {
	LOCK(m_db.getMutex());
	CHECK(sqlite3_step(m_stmt), m_db);
	return (uint32_t)sqlite3_last_insert_rowid(m_db);
}

//-------------------------------------------------------------------
uint64_t SQLStmt::step64(void) {
	LOCK(m_db.getMutex());
	CHECK(sqlite3_step(m_stmt), m_db);
	return sqlite3_last_insert_rowid(m_db);
}

//-------------------------------------------------------------------
SQLStmt& SQLStmt::bind(const SQLStmt& other) {
	reset();

	const uint32_t columnCount = getParameterCount();

	for(uint32_t col = 0; col < columnCount; col++) {
		const int type = other.getColumnType(col);

		switch(type) {
		case SQLITE_INTEGER: 
			bind(col, other.get<uint64_t>(col));
			break;
		case SQLITE_TEXT:
			bind(col, other.get<const char*>(col));
			break;
		case SQLITE_FLOAT:
			bind(col, other.get<double>(col));
			break;
		case SQLITE_BLOB:
			sqlite3_bind_blob(m_stmt, col+1, sqlite3_column_blob(other.m_stmt, col), sqlite3_column_bytes(other.m_stmt, col), SQLITE_STATIC);
			break;
		case SQLITE_NULL:
			sqlite3_bind_null(m_stmt, col+1);
			break;
		default:
			THROW("unknown column type");
		}
	}

	return *this;
}

//-------------------------------------------------------------------
	}
}