// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_DB_H
#define __MATOG_DB_H

#include <matog/dll.h>
#include <matog/templates/Singleton.h>
#include <matog/util/SQLiteDB.h>

namespace matog {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL DB : public util::SQLiteDB, public templates::Singleton<DB> {
private:
	static DB* s_instance;

					DB				(void);
	void			createTables	(void);
	void			checkTables		(void);

public:	  
	virtual						~DB								(void);	
	static	void				setDBFile						(const char* dbfile);
	static	void				setSetupNewDB					(const bool setupNewDB);
	static	void				setFailIfDoesNotExist			(const bool failIfDoesNotExist);
	static	const char*			getDefaultFileName				(void);

	friend class templates::Singleton<DB>;
};

//-------------------------------------------------------------------
}

#endif