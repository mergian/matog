SET(SRC
	analyze/training/Data.h
	analyze/training/Data.cpp
	analyze/training/Data_Index.cpp
	analyze/training/Training.h
	analyze/training/Training.cpp
)

SOURCE_GROUP("analyze\\training" FILES ${SRC})
LIST(APPEND ANALYZE_SRC ${SRC})