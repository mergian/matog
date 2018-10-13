{{!
Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
Use of this source code is governed by the BSD 3-Clause license that can be
found in the LICENSE file. 
}}# {{LIB_NAME}}/CMakeLists.txt

{{! minimum cmake version }}
CMAKE_MINIMUM_REQUIRED(VERSION {{MIN_VERSION}})

{{! required settings }}
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

{{! set }}
{{#SETS}}SET({{SET_KEY}} {{SET_VALUE}})
{{/SETS}}

IF(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
ENDIF()

{{! find modules }}
{{#FIND_PATHS}}SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "{{FIND_PATH}}")
{{/FIND_PATHS}}
{{#FIND_MODULES}}FIND_PACKAGE({{MODULE_NAME}} {{MODULE_VERSION}} {{MODULE_REQUIRED}} {{MODULE_COMPONENTS}})
{{/FIND_MODULES}}

{{! definitions }}
{{#DEFINITIONS}}ADD_DEFINITIONS("{{DEFINITION}}")
{{/DEFINITIONS}}

{{! link directories }}
{{#LINK_DIRECTORIES}}LINK_DIRECTORIES("{{LINK_DIRECTORY}}")
{{/LINK_DIRECTORIES}}

{{! include directories }}
{{#INCLUDE_DIRECTORIES}}INCLUDE_DIRECTORIES("{{INCLUDE_DIRECTORY}}")
{{/INCLUDE_DIRECTORIES}}

{{! cuda include directories }}
{{#CUDA_INCLUDE_DIRECTORIES}}LINK_DIRECTORIES("{{CUDA_INCLUDE_DIRECTORY}}")
{{/CUDA_INCLUDE_DIRECTORIES}}

{{! pre includes }}
{{#PRE_INCLUDES}}INCLUDE("{{PRE_INCLUDE}}")
{{/PRE_INCLUDES}}

{{! sub directories }}
{{#SUB_DIRECTORIES}}INCLUDE("{{SUB_DIRECTORY}}")
{{/SUB_DIRECTORIES}}

{{! source groups }}
{{#SOURCE_GROUPS}}SOURCE_GROUP({{SOURCE_GROUP}} FILES 
{{SOURCE_FILES}})
{{/SOURCE_GROUPS}}

{{! declare header files, so they won't be compiled }}
SET_SOURCE_FILES_PROPERTIES(
{{HEADER_FILES}} 
	PROPERTIES HEADER_FILE_ONLY TRUE
)

# add target
CUDA_ADD_LIBRARY({{LIB_NAME}} {{SOURCE_FILES}})
TARGET_LINK_LIBRARIES({{LIB_NAME}}{{#LINK_LIBRARIES}} {{LINK_LIBRARY}}{{/LINK_LIBRARIES}})