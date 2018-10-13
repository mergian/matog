INCLUDE(cuda/jit/CMakeLists.txt)

SET(SRC
	cuda/CUDA.h
	cuda/CUDA.cpp
	cuda/Architecture.h
	cuda/NVDevice.h
	cuda/NVDevice.cpp
	cuda/Jit.h
	cuda/Jit.cpp
	cuda/CUBINStore.h
	cuda/CUBINStore.cpp
	cuda/Occupancy.h
	cuda/Occupancy.cpp
)

SOURCE_GROUP("cuda" FILES ${SRC})
LIST(APPEND INTERNAL_SRC ${SRC})