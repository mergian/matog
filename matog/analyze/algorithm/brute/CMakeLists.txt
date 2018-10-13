SET(SRC
	analyze/algorithm/brute/Brute.h
	analyze/algorithm/brute/Brute.cpp
)

SOURCE_GROUP("analyze\\algorithm\\brute" FILES ${SRC})
LIST(APPEND ANALYZE_SRC ${SRC})