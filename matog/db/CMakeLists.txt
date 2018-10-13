SET(SRC
	db/Stmt.cpp
	db/Stmt.h
	db/tables.h
	db/Clear.h
	db/Clear.cpp
	db/PredictionModel.h
	db/PredictionModel.cpp
)

SOURCE_GROUP("db" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})