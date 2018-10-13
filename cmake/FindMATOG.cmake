# Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
# Use of this source code is governed by a BSD 3-Clause license that can be
# found in the LICENSE file. }}

IF(NOT MATOG_FOUND)
	SET(MATOG_FOUND "YES")

	MESSAGE(STATUS "Found MATOG. Using local project.")

	SET(MATOG_LIBRARIES 
		matog-lib
		matog-log
		matog-util
		sqlite3
		ctemplate
		jsoncpp
		${CUDA_cupti_LIBRARY}
	)
	
	SET(MATOG_INCLUDE_DIR
		"${CMAKE_SOURCE_DIR}/matog/lib"
		"${CMAKE_SOURCE_DIR}/matog/log"
	)
ENDIF()