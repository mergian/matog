INCLUDE(runtime/intern/exec/profiler/CMakeLists.txt)

SET(SRC
	runtime/intern/exec/Compiler.h
	runtime/intern/exec/Compiler.cpp
	runtime/intern/exec/Baseline.h
	runtime/intern/exec/Baseline.cpp
	runtime/intern/exec/Optimized.h
	runtime/intern/exec/Optimized.cpp
	runtime/intern/exec/Profiler.h
	runtime/intern/exec/Profiler.cpp
	runtime/intern/exec/Profiler_Static.cpp
	runtime/intern/exec/Params.h
	runtime/intern/exec/Mode.h
)

SOURCE_GROUP("runtime\\intern\\exec" FILES ${SRC})
LIST(APPEND RUNTIME_SRC ${SRC})