############################################################################
# CMakeLists.txt
# Copyright (C) 2016  Belledonne Communications, Grenoble France
#
############################################################################
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
############################################################################

cmake_minimum_required(VERSION 3.0)

project(libjpeg-turbo LANGUAGES C)

set(PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(VERSION 1.5.2)

string(REPLACE "." ";" VERSION_TRIPLET ${VERSION})
list(GET VERSION_TRIPLET 0 VERSION_MAJOR)
list(GET VERSION_TRIPLET 1 VERSION_MINOR)
list(GET VERSION_TRIPLET 2 VERSION_REVISION)
function(pad_number NUMBER OUTPUT_LEN)
	string(LENGTH "${${NUMBER}}" INPUT_LEN)
	if(INPUT_LEN LESS OUTPUT_LEN)
		math(EXPR ZEROES "${OUTPUT_LEN} - ${INPUT_LEN} - 1")
		set(NUM ${${NUMBER}})
		foreach(C RANGE ${ZEROES})
			set(NUM "0${NUM}")
		endforeach()
		set(${NUMBER} ${NUM} PARENT_SCOPE)
	endif()
endfunction()
pad_number(VERSION_MINOR 3)
pad_number(VERSION_REVISION 3)
set(LIBJPEG_TURBO_VERSION_NUMBER ${VERSION_MAJOR}${VERSION_MINOR}${VERSION_REVISION})
set(LIBJPEG_TURBO_VERSION "${LIBJPEG_TURBO_VERSION_NUMBER}")

string(TIMESTAMP BUILD "%Y%m%d")

option(ENABLE_SHARED "Build shared library." YES)
option(ENABLE_STATIC "Build static library." YES)

include(GNUInstallDirs)

set(SO_AGE 0)
set(JPEG_LIB_VERSION 62)
set(DLL_VERSION ${JPEG_LIB_VERSION})
set(FULLVERSION ${DLL_VERSION}.${SO_AGE}.0)

include(CheckTypeSize)
include(CheckIncludeFile)

check_type_size(size_t SIZEOF_SIZE_T)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(stddef.h HAVE_STDDEF_H)
check_include_file(locale.h HAVE_LOCALE_H)

foreach(KEYWORD "inline" "__inline__" "__inline")
	if(NOT DEFINED C_INLINE)
		try_compile(C_HAS_${KEYWORD} "${CMAKE_CURRENT_BINARY_DIR}"
			"${CMAKE_CURRENT_SOURCE_DIR}/test_inline.c"
		COMPILE_DEFINITIONS "-DINLINE=${KEYWORD}")
		if(C_HAS_${KEYWORD})
			set(C_INLINE TRUE)
			set(INLINE ${KEYWORD})
		endif()
	endif()
endforeach()

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	add_definitions("-DNO_GETENV" "-DNO_PUTENV")
endif()

configure_file(jconfig.h.cmake.in jconfig.h)
configure_file(jconfigint.h.cmake.in jconfigint.h)

set(JPEG_SOURCES
	jcapimin.c
	jcapistd.c
	jccoefct.c
	jccolor.c
	jcdctmgr.c
	jchuff.c
	jcinit.c
	jcmainct.c
	jcmarker.c
	jcmaster.c
	jcomapi.c
	jcparam.c
	jcphuff.c
	jcprepct.c
	jcsample.c
	jctrans.c
	jdapimin.c
	jdapistd.c
	jdatadst.c
	jdatasrc.c
	jdcoefct.c
	jdcolor.c
	jddctmgr.c
	jdhuff.c
	jdinput.c
	jdmainct.c
	jdmarker.c
	jdmaster.c
	jdmerge.c
	jdphuff.c
	jdpostct.c
	jdsample.c
	jdtrans.c
	jerror.c
	jfdctflt.c
	jfdctfst.c
	jfdctint.c
	jidctflt.c
	jidctfst.c
	jidctint.c
	jidctred.c
	jquant1.c
	jquant2.c
	jutils.c
	jmemmgr.c
	jmemnobs.c
	jsimd_none.c
)

include_directories("${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set(TURBOJPEG_SOURCES
	turbojpeg.c
	transupp.c
	jdatadst-tj.c
	jdatasrc-tj.c
)

if(ENABLE_SHARED)
	add_library(turbojpeg SHARED ${JPEG_SOURCES} ${TURBOJPEG_SOURCES})
	if(WIN32)
		set_target_properties(turbojpeg PROPERTIES DEFINE_SYMBOL DLLDEFINE)
	endif()
endif()

if(ENABLE_STATIC)
	add_library(turbojpeg STATIC ${JPEG_SOURCES} ${TURBOJPEG_SOURCES})
	if(NOT MSVC)
		set_target_properties(turbojpeg PROPERTIES OUTPUT_NAME turbojpeg)
	endif()
endif()

install(TARGETS turbojpeg
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/turbojpeg.h"
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
