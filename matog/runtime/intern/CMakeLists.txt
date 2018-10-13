INCLUDE(runtime/intern/context/CMakeLists.txt)
INCLUDE(runtime/intern/exec/CMakeLists.txt)
INCLUDE(runtime/intern/predictor/CMakeLists.txt)

SET(SRC
	runtime/intern/BaseArray.h
	runtime/intern/Context.cpp
	runtime/intern/Context.h
	runtime/intern/DeviceArray.cpp
	runtime/intern/DeviceArray.h
	runtime/intern/DynArray.cpp
	runtime/intern/DynArray.h
	runtime/intern/Exec.cpp
	runtime/intern/Exec.h
	runtime/intern/HostArray.h
	runtime/intern/MemArray.h
	runtime/intern/Predictor.cpp
	runtime/intern/Predictor.h
	runtime/intern/Runtime.cpp
	runtime/intern/Runtime.h
	runtime/intern/Runtime_Callback.cpp
	runtime/intern/Runtime_MemCpy.cpp
	runtime/intern/Decisions.h
	runtime/intern/Decisions.cpp
)

SOURCE_GROUP("runtime\\intern" FILES ${SRC})
LIST(APPEND RUNTIME_SRC ${SRC})