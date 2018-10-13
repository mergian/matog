SET(SRC
	analyze/algorithm/prediction/Prediction.h
	analyze/algorithm/prediction/Prediction.cpp
	analyze/algorithm/prediction/Helper.h
	analyze/algorithm/prediction/Helper.cpp
)

SOURCE_GROUP("analyze\\algorithm\\prediction" FILES ${SRC})
LIST(APPEND ANALYZE_SRC ${SRC})