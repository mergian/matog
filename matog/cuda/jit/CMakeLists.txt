SET(SRC
	cuda/jit/Item.h
	cuda/jit/Item.cpp
	cuda/jit/Target.h
	cuda/jit/Target.cpp
)

SOURCE_GROUP("cuda\\jit" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})