# Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
# Use of this source code is governed by the BSD 3-Clause license that can be
# found in the LICENSE file.
 
#------------------------------------------------------------------------------
# INIT CMAKE
#------------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(${CMAKE_MAJOR_VERSION} GREATER 2)
	CMAKE_POLICY(VERSION 2.8)
	CMAKE_POLICY(SET CMP0043 OLD)
ENDIF()

# PROJECT TITLE
PROJECT(MATOG)

# ENABLE PROJECT FOLDERS
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# SET BUILD TYPES
SET(CMAKE_CONFIGURATION_TYPES "DEBUG;RELEASE")

#------------------------------------------------------------------------------
# OVERWRITE GLOBAL FindMATOG.cmake!
#------------------------------------------------------------------------------
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#------------------------------------------------------------------------------
# ENSURE DLL ARE IN SAME FOLDER AS EXE ON WINDOWS
#------------------------------------------------------------------------------
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
    CACHE PATH "Single directory for all static libraries.")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
    CACHE PATH "Single directory for all dynamic libraries on Unix.")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
    CACHE PATH "Single directory for all executable and dynamic libraries on Windows.")
MARK_AS_ADVANCED(CMAKE_RUNTIME_OUTPUT_DIRECTORY CMAKE_LIBRARY_OUTPUT_DIRECTORY CMAKE_ARCHIVE_OUTPUT_DIRECTORY)

#------------------------------------------------------------------------------
# COMPILER SETTINGS
#------------------------------------------------------------------------------
IF(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -std=c++0x")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
ELSE()
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX /D_XKEYCHECK_H /MP /wd4251")
ENDIF()

#------------------------------------------------------------------------------
# FIND PACKAGES
#------------------------------------------------------------------------------

# CUDA
MESSAGE(STATUS "[Packages]")
MESSAGE(STATUS "-- CUDA")
FIND_PACKAGE(CUDA REQUIRED)
IF(NOT DEFINED ${CUDA_SDK_ROOT_DIR})
	SET(CUDA_SDK_ROOT_DIR $ENV{NVCUDASAMPLES_ROOT})
ENDIF()

SET(CUDA_INCLUDE_DIR "${CUDA_SDK_ROOT_DIR}/common/inc" "${CUDA_TOOLKIT_ROOT_DIR}/extras/CUPTI/include" ${CUDA_TOOLKIT_INCLUDE})
INCLUDE_DIRECTORIES(${CUDA_INCLUDE_DIR})

IF(NOT UNIX)
	LINK_DIRECTORIES("${CUDA_SDK_ROOT_DIR}/common/lib/x64")
ENDIF()

# TBB
MESSAGE(STATUS "-- TBB")
FIND_PACKAGE(TBB REQUIRED)
LINK_DIRECTORIES(${TBB_LIBRARY_DIRS})
INCLUDE_DIRECTORIES(${TBB_INCLUDE_DIR})

# OpenCV
MESSAGE(STATUS "-- OpenCV")
SET(OpenCV_STATIC OFF)
SET(OpenCV_DIR $ENV{OpenCV_DIR})
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

MESSAGE(STATUS "[/Packages]")

#------------------------------------------------------------------------------
# LIBRARIES
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_LIST_DIR}/libs")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_LIST_DIR}/libs/jsoncpp/include")
ADD_SUBDIRECTORY(libs)
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_LIST_DIR}/libs/ctemplate")

#------------------------------------------------------------------------------
# MATOG
#------------------------------------------------------------------------------
ADD_SUBDIRECTORY(matog)