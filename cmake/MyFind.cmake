#
# Copyright 2019 Pavel Babyak quarkdoe@gmail.com
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#



if( NOT PKGCONFIG_FOUND )
	find_package( PkgConfig REQUIRED )
	message( STATUS "PKGCONFIG_FOUND: ${PKGCONFIG_FOUND}")
endif()

macro( my_find_package _package )
	set( MacroArgs ${ARGN} )
	list( LENGTH MacroArgs MacroArgsNum )

	if( MacroArgsNum LESS 1 )
		message( SEND_ERROR "too fiew arguments for my_find_package" )
	endif()

	find_package( ${_package} ${MacroArgs} )

	message( STATUS "${_package}_FOUND                  : ${${_package}_FOUND}" )
	if( VERBOSE )
		message( STATUS "${_package}_LIBRARIES              : ${${_package}_LIBRARIES}" )
		message( STATUS "${_package}_LIBRARY_DIRS           : ${${_package}_LIBRARY_DIRS}" )
		message( STATUS "${_package}_LDFLAGS                : ${${_package}_LDFLAGS}" )
		message( STATUS "${_package}_LDFLAGS_OTHER          : ${${_package}_LDFLAGS_OTHER}" )
		message( STATUS "${_package}_INCLUDE_DIRS           : ${${_package}_INCLUDE_DIRS}" )
		message( STATUS "${_package}_CFLAGS                 : ${${_package}_CFLAGS}" )
		message( STATUS "${_package}_CFLAGS_OTHER           : ${${_package}_CFLAGS_OTHER}" )
		message( STATUS "${_package}_VERSION                : ${${_package}_VERSION}" )
	endif()
endmacro( my_find_package )

macro( my_pkg_check_modules _varname )

	set( MacroArgs ${ARGN} )
	list( LENGTH MacroArgs MacroArgsNum )

	if( MacroArgsNum LESS 1 )
		message( SEND_ERROR "too fiew arguments for my_pkg_check_modules" )
	endif()

	pkg_check_modules( ${_varname} ${MacroArgs} )

	message( STATUS "${_varname}_FOUND                  : ${${_varname}_FOUND}" )
	if( VERBOSE )
		message( STATUS "${_varname}_LIBRARIES              : ${${_varname}_LIBRARIES}" )
		message( STATUS "${_varname}_LIBRARY_DIRS           : ${${_varname}_LIBRARY_DIRS}" )
		message( STATUS "${_varname}_LDFLAGS                : ${${_varname}_LDFLAGS}" )
		message( STATUS "${_varname}_LDFLAGS_OTHER          : ${${_varname}_LDFLAGS_OTHER}" )
		message( STATUS "${_varname}_INCLUDE_DIRS           : ${${_varname}_INCLUDE_DIRS}" )
		message( STATUS "${_varname}_CFLAGS                 : ${${_varname}_CFLAGS}" )
		message( STATUS "${_varname}_CFLAGS_OTHER           : ${${_varname}_CFLAGS_OTHER}" )
		message( STATUS "${_varname}_VERSION                : ${${_varname}_VERSION}" )
	endif()
endmacro( my_pkg_check_modules )
