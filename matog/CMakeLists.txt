# Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
# Use of this source code is governed by the BSD 3-Clause license that can be
# found in the LICENSE file.

#------------------------------------------------------------------------------
# Set Paths and defines and init lists
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES("${CUDA_TOOLKIT_ROOT_DIR}/extras/CUPTI/include")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_LIST_DIR}/../")
ADD_DEFINITIONS(-DCTEMPLATE_DLL_DECL=)

SET(LOG_SRC)
SET(UTIL_SRC)
SET(INTERNAL_SRC)
SET(RUNTIME_SRC)
SET(CODE_SRC)
SET(ANALYZE_SRC)
SET(MISC_SRC)
SET(LOG_INSTALL)
SET(GLOBAL_INSTALL)
SET(RUNTIME_INSTALL)

#------------------------------------------------------------------------------
# Add files in current directory
#------------------------------------------------------------------------------
# Runtime
LIST(APPEND RUNTIME_SRC matog.h matog.cpp macros.h enums.h runtime.h)
LIST(APPEND GLOBAL_INSTALL matog.h macros.h enums.h)

# Internal
LIST(APPEND INTERNAL_SRC Array.h Array.cpp Array_Static.cpp DB.h DB.cpp Degree.h Degree.cpp Degree_Static.cpp GPU.h GPU.cpp Kernel.h Kernel.cpp Kernel_Static.cpp Module.h Module.cpp Options.h Options.cpp Static.h Static.cpp Variant.h Variant.cpp internal.h)
	
# Log
LIST(APPEND LOG_SRC log.h)
LIST(APPEND GLOBAL_INSTALL log.h)

# Util
List(APPEND UTIL_SRC util.h)

# Analyze
List(APPEND ANALYZE_SRC analyze.h)
	
# Source Groups
SOURCE_GROUP("" FILES ${RUNTIME_SRC} ${INTERNAL_SRC} ${UTIL_SRC} ${LOG_SRC} ${ANALYZE_SRC})

#------------------------------------------------------------------------------
# Include all sub directories
#------------------------------------------------------------------------------
INCLUDE(analyze/CMakeLists.txt)
INCLUDE(array/CMakeLists.txt)
INCLUDE(code/CMakeLists.txt)
INCLUDE(cuda/CMakeLists.txt)
INCLUDE(db/CMakeLists.txt)
INCLUDE(degree/CMakeLists.txt)
INCLUDE(event/CMakeLists.txt)
INCLUDE(kernel/CMakeLists.txt)
INCLUDE(log/CMakeLists.txt)
INCLUDE(runtime/CMakeLists.txt)
INCLUDE(templates/CMakeLists.txt)
INCLUDE(util/CMakeLists.txt)
INCLUDE(variant/CMakeLists.txt)
INCLUDE(misc/CMakeLists.txt)

#------------------------------------------------------------------------------
# matog-log
#------------------------------------------------------------------------------
ADD_LIBRARY(matog-log SHARED ${LOG_SRC})
TARGET_LINK_LIBRARIES(matog-log
	${CUDA_CUDA_LIBRARY} 
	${CUDA_cupti_LIBRARY}
	matog-sqlite3
)

SET_PROPERTY(TARGET matog-log PROPERTY FOLDER "libs")

#------------------------------------------------------------------------------
# matog-util
#------------------------------------------------------------------------------
ADD_LIBRARY(matog-util SHARED ${UTIL_SRC})
TARGET_LINK_LIBRARIES(matog-util 
	matog-sqlite3
	matog-log
	opencv_ml
)

SET_PROPERTY(TARGET matog-util PROPERTY FOLDER "libs")

#------------------------------------------------------------------------------
# matog-internal
#------------------------------------------------------------------------------
ADD_LIBRARY(matog-internal SHARED ${INTERNAL_SRC})
TARGET_LINK_LIBRARIES(matog-internal
	matog-util
	matog-log
	matog-jsoncpp
)

SET_PROPERTY(TARGET matog-internal PROPERTY FOLDER "libs")

#------------------------------------------------------------------------------
# matog-runtime
#------------------------------------------------------------------------------
ADD_LIBRARY(matog-runtime SHARED ${RUNTIME_SRC})
TARGET_LINK_LIBRARIES(matog-runtime
	matog-internal
	matog-util
	matog-log
	tbb
)

SET_PROPERTY(TARGET matog-runtime PROPERTY FOLDER "libs")

#------------------------------------------------------------------------------
# matog-code
#------------------------------------------------------------------------------
ADD_EXECUTABLE(matog-code ${CODE_SRC} ${CODE_TPLS} ${FINDMATOG})
TARGET_LINK_LIBRARIES(matog-code
	matog-util
	matog-log
	matog-internal
	matog-ctemplate)
ADD_CUSTOM_COMMAND(TARGET matog-code COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_LIST_DIR}/matog/tpl" "${CMAKE_BINARY_DIR}/tpl" DEPENDS ${CODE_TPLS} ${CMAKE_TPLS})

SET_PROPERTY(TARGET matog-code PROPERTY FOLDER "executables")

#------------------------------------------------------------------------------
# matog-analyze
#------------------------------------------------------------------------------
ADD_EXECUTABLE(matog-analyze ${ANALYZE_SRC})
TARGET_LINK_LIBRARIES(matog-analyze
	matog-util
	matog-log
	matog-internal
	tbb
)

SET_PROPERTY(TARGET matog-analyze PROPERTY FOLDER "executables")

#------------------------------------------------------------------------------
# matog-misc
#------------------------------------------------------------------------------
ADD_EXECUTABLE(matog-misc ${MISC_SRC})
TARGET_LINK_LIBRARIES(matog-misc
	matog-util
	matog-log
	matog-internal
	tbb
)

SET_PROPERTY(TARGET matog-misc PROPERTY FOLDER "executables")

#------------------------------------------------------------------------------
# Install Header and Lib files
#------------------------------------------------------------------------------
INSTALL(FILES ${GLOBAL_INSTALL} 		DESTINATION include/matog)
INSTALL(FILES ${LOG_INSTALL} 			DESTINATION include/matog/log)
INSTALL(FILES ${RUNTIME_INSTALL} 		DESTINATION include/matog/runtime)
INSTALL(FILES ${CODE_TPLS} 				DESTINATION "bin/tpl")
INSTALL(FILES ${FINDMATOG} 				DESTINATION "./")

INSTALL(TARGETS matog-log RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
INSTALL(TARGETS matog-util RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
INSTALL(TARGETS matog-internal RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
INSTALL(TARGETS matog-runtime RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
INSTALL(TARGETS matog-code RUNTIME DESTINATION bin)
INSTALL(TARGETS matog-analyze RUNTIME DESTINATION bin)