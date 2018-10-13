# Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
# Use of this source code is governed by the BSD 3-Clause license that can be
# found in the LICENSE file.

SET(SRC
	log/Logger.cpp
	log/Result.cpp
)

SET(HEADER
	log/Logger.h
	log/Timer.h
	log/dll.h
	log/Error.h
	log/Result.h
)

SOURCE_GROUP("log" FILES ${SRC} ${HEADER})
LIST(APPEND LOG_SRC ${SRC} ${HEADER})
LIST(APPEND LOG_INSTALL ${HEADER})