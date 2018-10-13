SET(SRC
	event/Type.h
	event/ProfilingAlgorithm.h
	event/event.cpp
)

SOURCE_GROUP("event" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})