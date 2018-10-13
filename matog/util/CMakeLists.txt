# Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
# Use of this source code is governed by the BSD 3-Clause license that can be
# found in the LICENSE file.

INCLUDE(util/ml/CMakeLists.txt)

SET(SRC
	util/dll.h
	util/Env.h
	util/Env.cpp
	util/Exit.h
	util/Exit.cpp
	util/Hash.h
	util/Hash.cpp
	util/IO.h
	util/IO.cpp
	util/Mem.h
	util/Mem.cpp
	util/Random.h
	util/SQLiteDB.h
	util/SQLiteDB.cpp
	util/SQLStmt.h
	util/SQLStmt.cpp
	util/String.h
	util/String.cpp
)

SOURCE_GROUP("util" FILES ${SRC})
LIST(APPEND UTIL_SRC ${SRC})