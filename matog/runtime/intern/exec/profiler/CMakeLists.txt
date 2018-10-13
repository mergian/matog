SET(SRC
	runtime/intern/exec/profiler/AllocStore.h
	runtime/intern/exec/profiler/AllocStore.cpp
	runtime/intern/exec/profiler/ArrayStore.h
	runtime/intern/exec/profiler/ArrayStore.cpp
	runtime/intern/exec/profiler/Brute.h
	runtime/intern/exec/profiler/Brute.cpp
	runtime/intern/exec/profiler/CUPTI.h
	runtime/intern/exec/profiler/CUPTI.cpp
	runtime/intern/exec/profiler/Plain.h
	runtime/intern/exec/profiler/Plain.cpp
	runtime/intern/exec/profiler/Prediction.h
	runtime/intern/exec/profiler/Prediction.cpp
	runtime/intern/exec/profiler/Converter.h
	runtime/intern/exec/profiler/Converter.cpp
	runtime/intern/exec/profiler/PredictionRandom.h
	runtime/intern/exec/profiler/PredictionRandom.cpp
)

SOURCE_GROUP("runtime\\intern\\exec\\profiler" FILES ${SRC})
LIST(APPEND RUNTIME_SRC ${SRC})