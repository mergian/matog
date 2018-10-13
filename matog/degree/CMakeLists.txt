SET(SRC
	degree/Set.h
	degree/Set.cpp
	degree/Type.h
	degree/PrettyPrinter.h
	degree/PrettyPrinter.cpp
)

SOURCE_GROUP("degree" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})