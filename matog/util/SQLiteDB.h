// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_UTIL_SQLITEDB
#define __MATOG_UTIL_SQLITEDB

#include "dll.h"
#include <sqlite3/sqlite3.h>
#include <map>

namespace std {
	class mutex;
}

namespace matog {
	namespace util {
//-------------------------------------------------------------------
/// Helper class for SQLite
class MATOG_UTIL_DLL SQLiteDB {
private:
					std::mutex*			m_mutex;				///< protects db for multithreaded access
					sqlite3*			m_db;					///< database connection
	const			char*				m_dbFile;				///< database file
					bool				m_failIfDoesNotExist;	///< fails if database file does not exist
					bool				m_transactionActive;	///< true if transaction is active

protected:
	/// open database
						SQLiteDB		(const char* dbFile, const bool failIfDoesNotExist);

	/// delete database file
			void		deleteDBFile	(void);

	/// open database
			void		openDB			(void);

	/// close database
			void		closeDB			(void);

	/// returns mutex
			std::mutex&	getMutex		(void);
			
public:	    	
	/// destroyes database object
	virtual				~SQLiteDB		(void);

	/// executes VACUUM
			void		vacuum			(void);

	/// checks database integrity
			void		 checkIntegrity	(void);

	/// start transaction
			void		startTransaction(void);

	/// end transaction
			void		endTransaction	(void);

	/// attach other db
			void		attach			(const char* db, const char* as);

	/// detach other db
			void		detach			(const char* as);

	/// returns connection
			operator sqlite3* (void);

	/// friends
			friend class SQLStmt;
};

//-------------------------------------------------------------------
	}
}

#endif