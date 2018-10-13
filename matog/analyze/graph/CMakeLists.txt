INCLUDE (analyze/graph/node/CMakeLists.txt)
INCLUDE (analyze/graph/call/CMakeLists.txt)

SET(SRC
	analyze/graph/Graph.h
	analyze/graph/Graph.cpp
	analyze/graph/Graph_Static.cpp
	analyze/graph/Node.h
	analyze/graph/Node.cpp
	analyze/graph/Decision.h
	analyze/graph/Decision.cpp
	analyze/graph/Call.h
	analyze/graph/Call.cpp
)

SOURCE_GROUP("analyze\\graph" FILES ${SRC})
LIST(APPEND ANALYZE_SRC ${SRC})