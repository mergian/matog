SET(SRC
	analyze/graph/node/Compare.h
	analyze/graph/node/Compare.cpp
)

SOURCE_GROUP("analyze\\graph\\node" FILES ${SRC})
LIST(APPEND ANALYZE_SRC ${SRC})