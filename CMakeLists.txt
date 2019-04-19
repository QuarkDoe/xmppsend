cmake_minimum_required( VERSION 3.0 )

project( xmppsend )

option( VERBOSE            "Be verbose" OFF )

SET( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

include( CheckIncludeFile )
include( CheckIncludeFileCXX )
include( MyFind )

set( CMAKE_CXX_STANDARD 11 )

my_pkg_check_modules( LIBCONFIG REQUIRED libconfig++>=1.4.8 )
include_directories( ${LIBCONFIG_INCLUDE_DIRS} )
link_directories( ${LIBCONFIG_LIBRARY_DIRS} )

my_pkg_check_modules( GLOOX REQUIRED gloox>=1.0.22 )
include_directories( ${GLOOX_INCLUDE_DIRS} )
link_directories( ${GLOOX_LIBRARY_DIRS} )

check_include_file( getopt.h HAVE_GETOPT_H )
message( STATUS "HAVE_GETOPT_H  : ${HAVE_GETOPT_H}" )
if( NOT HAVE_GETOPT_H )
	add_subdirectory( getopt )
endif( NOT HAVE_GETOPT_H )

add_executable( xmppsend main.cpp messenger.cpp mucsender.cpp format.cpp )

set_target_properties( xmppsend PROPERTIES LINKER_LANGUAGE CXX )

target_link_libraries(
	xmppsend
	pthread
	${GLOOX_LIBRARIES}
	${LIBCONFIG_LIBRARIES}
)

install(TARGETS xmppsend RUNTIME DESTINATION bin)
