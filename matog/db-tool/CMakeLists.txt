# Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
# Use of this source code is governed by the BSD 3-Clause license that can be
# found in the LICENSE file.

SET(SOURCE_FILES
	main.cpp
	matog/db/DBUtil.cpp
	#matog/db/Predict.cpp
	matog/db/Info.cpp
	#matog/db/Merge.cpp
	#matog/db/XPredict.cpp
	#matog/db/XAnalyse.cpp
	#matog/db/RegDump.cpp
	#matog/db/XBestVariant.cpp
	#matog/db/XBaseLine.cpp
	#matog/db/XWatchDogOverhead.cpp
)

SET(HEADER_FILES
	#matog/db/XPredict.h
	#matog/db/XAnalyse.h
	#matog/db/Predict.h
	matog/db/DBUtil.h
	matog/db/Info.h
	#matog/db/Merge.h
	#matog/db/RegDump.h
	#matog/db/XBestVariant.h
	#matog/db/XBaseLine.h
	#matog/db/XWatchDogOverhead.h
	matog/db.h
)

CUDA_ADD_EXECUTABLE(matog-db ${SOURCE_FILES} ${HEADER_FILES} ${CODE_TPLS} ${FINDMATOG})
TARGET_LINK_LIBRARIES(matog-db
	matog-util
	matog-log
	matog-lib
)

SET_PROPERTY(TARGET matog-db PROPERTY FOLDER "executables")
INSTALL(TARGETS matog-db RUNTIME DESTINATION bin)