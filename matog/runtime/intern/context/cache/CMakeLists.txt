SET(SRC
	runtime/intern/context/cache/Item.h
	runtime/intern/context/cache/Item.cpp
)

SOURCE_GROUP("runtime\\intern\\context\\cache" FILES ${SRC})
LIST(APPEND RUNTIME_SRC ${SRC})