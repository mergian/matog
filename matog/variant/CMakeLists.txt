SET(SRC
	variant/Item.h
	variant/Item.cpp
	variant/Iterator.h
	variant/Iterator.cpp
	variant/Print.h
	variant/Print.cpp
	variant/Type.h
	variant/Index.h
	variant/Mul.h
	variant/Set.h
	variant/Items.h
)

SOURCE_GROUP("variant" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})