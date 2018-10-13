INCLUDE(array/field/CMakeLists.txt)

SET(SRC
	array/Set.h
	array/Set.cpp
	array/Field.h
	array/Field.cpp
	array/Type.h
)

SOURCE_GROUP("array" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})