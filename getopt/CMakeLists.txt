#
cmake_minimum_required( VERSION 2.8 )

project( GETOPT )
message( STATUS "[${PROJECT_NAME}]" )
message( STATUS "Because we do not use standard getopt." )

include_directories( . )

add_library( getopt STATIC getopt.c )

set_target_properties( getopt PROPERTIES LINKER_LANGUAGE C )
