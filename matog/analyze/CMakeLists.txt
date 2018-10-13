INCLUDE(analyze/algorithm/CMakeLists.txt)
#INCLUDE(analyze/compiler/CMakeLists.txt)
INCLUDE(analyze/graph/CMakeLists.txt)
INCLUDE(analyze/training/CMakeLists.txt)

SET(SRC
	analyze/Analyze.h
	analyze/Analyze.cpp
	analyze/main.cpp
)

SOURCE_GROUP("analyze" FILES ${SRC})
LIST(APPEND ANALYZE_SRC ${SRC})