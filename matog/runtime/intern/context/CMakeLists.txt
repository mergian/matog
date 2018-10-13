INCLUDE(runtime/intern/context/cache/CMakeLists.txt)

SET(SRC
	runtime/intern/context/Cache.h
	runtime/intern/context/Cache.cpp
	runtime/intern/context/Meta.h
	runtime/intern/context/Meta.cpp
)

SOURCE_GROUP("runtime\\intern\\context" FILES ${SRC})
LIST(APPEND RUNTIME_SRC ${SRC})