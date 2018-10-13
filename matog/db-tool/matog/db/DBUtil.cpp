// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/shared.h>
#include <matog/log.h>
#include <matog/util.h>
#include <matog/cuda.h>
#include "../db.h"
#include <cstdio>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
DBUtil::DBUtil(int argc, char** argv) {
	if(argc < 3) {
		L_INFO("usage: matog-db db-file [clear-db, clear-cubins, predict, info] ...");
		return;
	}

	DB::setDBFile(argv[1]);
	DB::setFailIfDoesNotExist(true);

#define IF(str) if(strcmp(argv[2], str) == 0)

	IF("clear-db")
		clearDB();
	else IF("clear-cubins")
		clearCubins();
	else IF("predict")
		THROW("we have to update the code to the new db structure...")//Predict(argc - 3, argv + 3);
	else IF("info")
		Info(argc - 3, argv + 3);
	else IF("merge")
		THROW("we have to update the code to the new db structure...")//Merge(argc - 3, argv + 3);
	else
		THROW("UNKNOWN_ARGS");

#undef IF
}

//-------------------------------------------------------------------
void DBUtil::vacuum(void) {
	L_INFO("vacuuming...");
	DB::getInstance().vacuum();
	return DB::getInstance().checkIntegrity();
}

//-------------------------------------------------------------------
void DBUtil::clearDB(void) {
	L_INFO("clearing db...");
	Clear::all();
	return vacuum();
}

//-------------------------------------------------------------------
void DBUtil::clearCubins(void) {
	L_INFO("clearing cubins...");
	DB::Stmt("DELETE FROM " DB_CUBINS ";").step();
	return vacuum();
}

//-------------------------------------------------------------------
	}
}