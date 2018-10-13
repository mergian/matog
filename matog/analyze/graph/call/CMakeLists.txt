SET(SRC
	analyze/graph/call/Item.h
	analyze/graph/call/Item.cpp
)

SOURCE_GROUP("analyze\\graph\\call" FILES ${SRC})
LIST(APPEND ANALYZE_SRC ${SRC})