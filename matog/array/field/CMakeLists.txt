SET(SRC
	array/field/Memory.h
	array/field/Type.h
	array/field/Type.cpp
	array/field/VType.h
	array/field/VType.cpp
)

SOURCE_GROUP("array\\field" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})