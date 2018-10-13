if( CMAKE_COMPILER_IS_GNUCXX )
  #Get compiler version.
  execute_process( COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                   OUTPUT_VARIABLE GNUCXX_VERSION )

  #-Werror=* was introduced -after- GCC 4.1.2
  if( GNUCXX_VERSION VERSION_GREATER 4.1.2 )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=strict-aliasing")
  endif()
endif( CMAKE_COMPILER_IS_GNUCXX )

SET( JSONCPP_INCLUDE_DIR ../include )

SET( PUBLIC_HEADERS
    ${JSONCPP_INCLUDE_DIR}/json/config.h
    ${JSONCPP_INCLUDE_DIR}/json/forwards.h
    ${JSONCPP_INCLUDE_DIR}/json/features.h
    ${JSONCPP_INCLUDE_DIR}/json/value.h
    ${JSONCPP_INCLUDE_DIR}/json/reader.h
    ${JSONCPP_INCLUDE_DIR}/json/writer.h
    ${JSONCPP_INCLUDE_DIR}/json/assertions.h
    ${JSONCPP_INCLUDE_DIR}/json/version.h
)

SET(jsoncpp_sources
	json_tool.h
	json_reader.cpp
	json_valueiterator.inl
	json_value.cpp
	json_writer.cpp
	version.h.in)

ADD_LIBRARY(matog-jsoncpp SHARED ${PUBLIC_HEADERS} ${jsoncpp_sources})

SET_PROPERTY(TARGET matog-jsoncpp PROPERTY FOLDER "libs-3rd-party")

IF(UNIX)
	SET_PROPERTY(TARGET matog-jsoncpp APPEND PROPERTY COMPILE_FLAGS -fPIC)
ENDIF()

INSTALL(TARGETS matog-jsoncpp RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)