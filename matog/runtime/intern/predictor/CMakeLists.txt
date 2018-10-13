SET(SRC
	runtime/intern/predictor/Meta.h
	runtime/intern/predictor/Meta.cpp
	runtime/intern/predictor/Item.h
)

SOURCE_GROUP("runtime\\intern\\predictor" FILES ${SRC})
LIST(APPEND RUNTIME_SRC ${SRC})