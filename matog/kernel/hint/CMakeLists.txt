SET(SRC
	kernel/hint/RWHints.cpp
	kernel/hint/RWHints.h
	kernel/hint/CountHints.cpp
	kernel/hint/CountHints.h
	kernel/hint/RefHints.cpp
	kernel/hint/RefHints.h
	kernel/hint/Identifier.h
)

SOURCE_GROUP("kernel\\hint" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})