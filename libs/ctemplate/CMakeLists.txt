MESSAGE(STATUS "  CTemplate:		1.X")

IF(WIN32)
	INCLUDE_DIRECTORIES("${CMAKE_CURRENT_LIST_DIR}/windows")
	SET(WINDOWS_SRC
		windows/config.h
		windows/port.cc
		windows/port.h
		windows/ctemplate/find_ptr.h
		windows/ctemplate/per_expand_data.h
		windows/ctemplate/str_ref.h
		windows/ctemplate/template.h
		windows/ctemplate/template_annotator.h
		windows/ctemplate/template_cache.h
		windows/ctemplate/template_dictionary.h
		windows/ctemplate/template_dictionary_interface.h
		windows/ctemplate/template_emitter.h
		windows/ctemplate/template_enums.h
		windows/ctemplate/template_modifiers.h
		windows/ctemplate/template_namelist.h
		windows/ctemplate/template_pathops.h
		windows/ctemplate/template_string.h
	)
ELSE()
	INCLUDE_DIRECTORIES("${CMAKE_CURRENT_LIST_DIR}/unix")
	SET(UNIX_SRC
		unix/config.h
		unix/ctemplate/find_ptr.h
		unix/ctemplate/per_expand_data.h
		unix/ctemplate/str_ref.h
		unix/ctemplate/template.h
		unix/ctemplate/template_annotator.h
		unix/ctemplate/template_cache.h
		unix/ctemplate/template_dictionary.h
		unix/ctemplate/template_dictionary_interface.h
		unix/ctemplate/template_emitter.h
		unix/ctemplate/template_enums.h
		unix/ctemplate/template_modifiers.h
		unix/ctemplate/template_namelist.h
		unix/ctemplate/template_pathops.h
		unix/ctemplate/template_string.h
	)
	
	FIND_PACKAGE(Threads)
ENDIF()

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_LIST_DIR}")

SET(SRC
	ctemplate/find_ptr.h
	ctemplate/per_expand_data.h
	ctemplate/str_ref.h
	ctemplate/template.h
	ctemplate/template_annotator.h
	ctemplate/template_cache.h
	ctemplate/template_dictionary.h
	ctemplate/template_dictionary_interface.h
	ctemplate/template_emitter.h
	ctemplate/template_enums.h
	ctemplate/template_modifiers.h
	ctemplate/template_namelist.h
	ctemplate/template_pathops.h
	ctemplate/template_string.h
	#diff_tpl_auto_escape.cc
	indented_writer.h
	#make_tpl_varnames_h.cc
	per_expand_data.cc
	template.cc
	template_annotator.cc
	template_cache.cc
	template_dictionary.cc
	template_modifiers.cc
	template_modifiers_internal.h
	template_namelist.cc
	template_pathops.cc
	template_string.cc
	base/arena.cc
	base/arena.h
	base/arena-inl.h
	base/fileutil.h
	base/macros.h
	base/manual_constructor.h
	base/mutex.h
	base/small_map.h
	base/thread_annotations.h
	base/util.h
	htmlparser/htmlparser.cc
	htmlparser/htmlparser_cpp.h
	htmlparser/htmlparser_fsm.h
	htmlparser/jsparser.cc
	htmlparser/jsparser.h
	htmlparser/jsparser_fsm.h
	htmlparser/statemachine.cc
	htmlparser/statemachine.h
)

ADD_LIBRARY(matog-ctemplate SHARED ${SRC} ${WINDOWS_SRC} ${UNIX_SRC})
SET_PROPERTY(TARGET matog-ctemplate PROPERTY FOLDER "libs-3rd-party")

IF(UNIX)
	TARGET_LINK_LIBRARIES(matog-ctemplate ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
ENDIF()

INSTALL(TARGETS matog-ctemplate RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)