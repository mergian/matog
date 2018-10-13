CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

MESSAGE(STATUS "  JsonCpp:		1.7.4")

# Include our configuration header
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_LIST_DIR}/include )

if ( MSVC )
    # Only enabled in debug because some old versions of VS STL generate
    # unreachable code warning when compiled in release configuration.
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 ")
endif( MSVC )

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # using GCC
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -pedantic")
endif()

# Build the different applications
ADD_SUBDIRECTORY(src)
