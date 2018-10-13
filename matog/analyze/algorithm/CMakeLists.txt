#INCLUDE(analyze/algorithm/iterative/CMakeLists.txt)
INCLUDE(analyze/algorithm/brute/CMakeLists.txt)
INCLUDE(analyze/algorithm/prediction/CMakeLists.txt)

SET(SRC
	analyze/algorithm/Algorithm.h
	analyze/algorithm/Algorithm.cpp
	analyze/algorithm/Type.h
)

SOURCE_GROUP("analyze\\algorithm" FILES ${SRC})
LIST(APPEND ANALYZE_SRC ${SRC})