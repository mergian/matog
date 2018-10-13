INCLUDE(runtime/intern/CMakeLists.txt)

SET(HEADER
	runtime/DeviceArray.h
	runtime/DynArray.h
	runtime/HostArray.h
	runtime/BaseArray.h
	runtime/MemArray.h
	runtime/Malloc.h
	runtime/dll.h
)

SET(SRC
	runtime/BaseArray.cpp
	runtime/DeviceArray.cpp
	runtime/DynArray.cpp
	runtime/Malloc.cpp
	runtime/MemArray.cpp
	runtime/HostArray.cpp
)

SOURCE_GROUP("runtime" FILES ${SRC} ${HEADER})
LIST(APPEND RUNTIME_SRC ${SRC} ${HEADER})
LIST(APPEND RUNTIME_INSTALL ${HEADER})