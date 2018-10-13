// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <sqlite3/sqlite3.h>
#include <matog/util/SQLiteDB.h>
#include <matog/util/SQLStmt.h>
#include <matog/util/String.h>
#include <matog/util/Mem.h>
#include <matog/util/IO.h>

#include <mutex>
#include <iomanip>
#include <matog/macros.h>

using namespace matog::log;
using namespace matog::util;

#ifdef NDEBUG
//#define USE_TRANSACTION
#endif

namespace matog {
	namespace util {
//-------------------------------------------------------------------
SQLiteDB::SQLiteDB(const char* dbFile, const bool failIfDoesNotExist) : 
	m_mutex				(0), 
	m_db				(0), 
	m_dbFile			(0), 
	m_failIfDoesNotExist(failIfDoesNotExist), 
	m_transactionActive	(false) 
{
	assert(dbFile);

	m_mutex		= NEW(std::mutex);
	m_dbFile	= NEW_STRING(dbFile);
}

//-------------------------------------------------------------------
SQLiteDB::operator sqlite3*(void) {
	return m_db;
}

//-------------------------------------------------------------------
void SQLiteDB::closeDB(void) {
	if(m_db) {
		#ifdef USE_TRANSACTION
		SQLStmt("COMMIT;", *this).step();
		#endif

		CHECK(sqlite3_close(m_db), m_db);
		
		m_db = 0;
	}
}

//-------------------------------------------------------------------
void SQLiteDB::deleteDBFile(void) {
	THROWIF(IO::doesFileExist(m_dbFile) && !IO::rm(m_dbFile), SQLITE_ERROR, m_db);
}

//-------------------------------------------------------------------
void SQLiteDB::openDB(void) {
	THROWIF(m_failIfDoesNotExist && !IO::doesFileExist(m_dbFile), SQLITE_ERROR, m_db);

	CHECK(sqlite3_open(m_dbFile, &m_db), m_db);
	
	// set sync mode
	SQLStmt("PRAGMA synchronous = 0;", *this).step();
	
	// set journal mode
	SQLStmt("PRAGMA journal_mode = OFF;", *this).step();
	
	#ifdef USE_TRANSACTION
	startTransaction();
	#endif
}

//-------------------------------------------------------------------
void SQLiteDB::startTransaction(void) {
	THROWIF(m_transactionActive, "SQL_TRANSCATION_ALREADY_ALIVE");
	
	SQLStmt("BEGIN EXCLUSIVE;", *this).step();
	m_transactionActive = true;
}

//-------------------------------------------------------------------
void SQLiteDB::endTransaction(void) {
	THROWIF(!m_transactionActive, "SQL_TRANSACTION_NOT_ALIVE");

	SQLStmt("COMMIT;", *this).step();
	m_transactionActive = false;
}

//-------------------------------------------------------------------
SQLiteDB::~SQLiteDB(void) {
	closeDB();

	if(m_mutex)
		FREE(m_mutex);

	if(m_dbFile)
		FREE_A(m_dbFile);
}

//-------------------------------------------------------------------
void SQLiteDB::vacuum(void) {
	#ifdef USE_TRANSACTION
	const bool transactionWasActive = m_transactionActive;

	if(transactionWasActive)
		endTransaction();
	#endif

	SQLStmt("VACUUM;", *this).step();

	#ifdef USE_TRANSACTION
	if(transactionWasActive)
		startTransaction();
	#endif
}

//-------------------------------------------------------------------
void SQLiteDB::checkIntegrity(void) {
	#ifdef USE_TRANSACTION
	const bool transactionWasActive = m_transactionActive;

	if(transactionWasActive)
		endTransaction();
	#endif

	SQLStmt("PRAGMA integrity_check;", *this).step();

	#ifdef USE_TRANSACTION
	if(transactionWasActive)
		startTransaction();
	#endif
}

//-------------------------------------------------------------------
void SQLiteDB::attach(const char* db, const char* as) {
	#ifdef USE_TRANSACTION
	const bool transactionWasActive = m_transactionActive;

	if(transactionWasActive)
		endTransaction();
	#endif

	assert(db);
	assert(as);

	SQLStmt stmt("ATTACH ? AS ?;", *this);
	stmt.bind(1, db);
	stmt.bind(2, as);
	stmt.step();

	#ifdef USE_TRANSACTION
	if(transactionWasActive)
		startTransaction();
	#endif
}

//-------------------------------------------------------------------
void SQLiteDB::detach(const char* as) {
	#ifdef USE_TRANSACTION
	const bool transactionWasActive = m_transactionActive;

	if(transactionWasActive)
		endTransaction();
	#endif

	assert(as);

	SQLStmt stmt("DETACH ?;", *this);
	stmt.bind(1, as);
	stmt.step();

	#ifdef USE_TRANSACTION
	if(transactionWasActive)
		startTransaction();
	#endif
}

//-------------------------------------------------------------------
std::mutex& SQLiteDB::getMutex(void) {
	return *m_mutex;
}

//-------------------------------------------------------------------
	}
}