INCLUDE(kernel/hint/CMakeLists.txt)

SET(SRC
	kernel/Meta.h
	kernel/Meta.cpp
	kernel/Code.h
	kernel/Code.cpp
)

SOURCE_GROUP("kernel" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})