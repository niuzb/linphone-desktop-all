############################################################################
# CMakeLists.txt
# Copyright (C) 2017  Belledonne Communications, Grenoble France
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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
############################################################################

cmake_minimum_required(VERSION 3.0)
project(xerces-c VERSION 3.1.4 LANGUAGES C CXX)


option(ENABLE_SHARED "Build shared library." YES)
option(ENABLE_STATIC "Build static library." YES)


include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CheckTypeSize)

set(STDC_HEADERS YES)
check_include_file("arpa/inet.h" HAVE_ARPA_INET_H)
check_include_file("fcntl.h" HAVE_FCNTL_H)
check_include_file("float.h" HAVE_FLOAT_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("langinfo.h" HAVE_LANGINFO_H)
check_include_file("limits.h" HAVE_LIMITS_H)
check_include_file("locale.h" HAVE_LOCALE_H)
check_include_file("memory.h" HAVE_MEMORY_H)
check_include_file("netdb.h" HAVE_NETDB_H)
check_include_file("netinet/in.h" HAVE_NETINET_IN_H)
check_include_file("nl_types.h" HAVE_NL_TYPES_H)
check_include_file("stddef.h" HAVE_STDDEF_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("strings.h" HAVE_STRINGS_H)
check_include_file("sys/param.h" HAVE_SYS_PARAM_H)
check_include_file("sys/socket.h" HAVE_SYS_SOCKET_H)
check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_include_file("sys/timeb.h" HAVE_SYS_TIMEB_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("wchar.h" HAVE_WCHAR_H)
check_include_file("wctype.h" HAVE_WCTYPE_H)
check_include_file("CoreServices/CoreServices.h" HAVE_CORESERVICES_CORESERVICES_H)
check_include_file("endian.h" HAVE_ENDIAN_H)
check_include_file("machine/endian.h" HAVE_MACHINE_ENDIAN_H)
check_include_file("arpa/nameser_compat.h" HAVE_ARPA_NAMESER_COMPAT_H)
check_include_file("cpuid.h" HAVE_CPUID_H)
check_include_file("ctype.h" HAVE_CTYPE_H)
check_include_file("dlfcn.h" HAVE_DLFCN_H)
check_include_file("errno.h" HAVE_ERRNO_H)
check_include_file("stdio.h" HAVE_STDIO_H)
check_include_file("winsock2.h" HAVE_WINSOCK2_H)

check_include_file_cxx("stdbool.h" HAVE_STDBOOL_H)

check_function_exists("getcwd" HAVE_GETCWD)
check_function_exists("pathconf" HAVE_PATHCONF)
check_function_exists("realpath" HAVE_REALPATH)
check_function_exists("getaddrinfo" HAVE_GETADDRINFO)
check_function_exists("gethostbyaddr" HAVE_GETHOSTBYADDR)
check_function_exists("gethostbyname" HAVE_GETHOSTBYNAME)
check_function_exists("socket" HAVE_SOCKET)
check_function_exists("clock_gettime" HAVE_CLOCK_GETTIME)
check_function_exists("ftime" HAVE_FTIME)
check_function_exists("gettimeofday" HAVE_GETTIMEOFDAY)
check_function_exists("memmove" HAVE_MEMMOVE)
check_function_exists("memset" HAVE_MEMSET)
check_function_exists("nl_langinfo" HAVE_NL_LANGINFO)
check_function_exists("setlocale" HAVE_SETLOCALE)
check_function_exists("localeconv" HAVE_LOCALECONV)
check_function_exists("strcasecmp" HAVE_STRCASECMP)
check_function_exists("strncasecmp" HAVE_STRNCASECMP)
check_function_exists("stricmp" HAVE_STRICMP)
check_function_exists("strnicmp" HAVE_STRNICMP)
check_function_exists("strchr" HAVE_STRCHR)
check_function_exists("strdup" HAVE_STRDUP)
check_function_exists("strrchr" HAVE_STRRCHR)
check_function_exists("strstr" HAVE_STRSTR)
check_function_exists("strtol" HAVE_STRTOL)
check_function_exists("strtoul" HAVE_STRTOUL)
check_function_exists("towupper" HAVE_TOWUPPER)
check_function_exists("towlower" HAVE_TOWLOWER)
check_function_exists("mblen" HAVE_MBLEN)
check_function_exists("wcsupr" HAVE_WCSUPR)
check_function_exists("wcslwr" HAVE_WCSLWR)
check_function_exists("wcsnicmp" HAVE_WCSNICMP)
check_function_exists("wcsicmp" HAVE_WCSICMP)

foreach(KEYWORD "inline" "__inline__" "__inline")
	if(NOT DEFINED C_INLINE)
		try_compile(C_HAS_${KEYWORD} "${CMAKE_CURRENT_BINARY_DIR}"
			"${CMAKE_CURRENT_SOURCE_DIR}/test_inline.c"
			COMPILE_DEFINITIONS "-Dinline=${KEYWORD}")
		if(C_HAS_${KEYWORD})
			set(C_INLINE TRUE)
			if(NOT "${KEYWORD}" STREQUAL "inline")
				set(inline ${KEYWORD})
			endif()
		endif()
	endif()
endforeach()

set(TYPES_CHECKED "int" "long" "long long" "short" "wchar_t" "__int64")
set(CMAKE_EXTRA_INCLUDE_FILES "stdint.h")
foreach(TYPE ${TYPES_CHECKED})
	string(TOUPPER ${TYPE} TYPE_SIZE_VAR)
	string(REPLACE " " "_" TYPE_SIZE_VAR "${TYPE_SIZE_VAR}")
	check_type_size(${TYPE} "SIZEOF_${TYPE_SIZE_VAR}")
endforeach()
set(CMAKE_EXTRA_INCLUDE_FILES)

set(HAVE_NAMESPACES YES)
set(HAVE_STD_LIBS YES)
set(HAVE_STD_NAMESPACE YES)
set(XERCES_SIZE_T size_t)
set(XERCES_SSIZE_T int32_t)
set(XERCES_U16BIT_INT uint16_t)
set(XERCES_U32BIT_INT uint32_t)
set(XERCES_U64BIT_INT uint64_t)
set(XERCES_S16BIT_INT int16_t)
set(XERCES_S32BIT_INT int32_t)
set(XERCES_S64BIT_INT int64_t)
set(XERCES_XMLCH_T wchar_t)
set(XERCES_HAS_CPP_NAMESPACE YES)
set(XERCES_STD_NAMESPACE YES)

set(APP_NO_THREADS YES)
set(XERCES_USE_MUTEXMGR_NOTHREAD YES)
if(WIN32)
	set(XERCES_USE_FILEMGR_WINDOWS YES)
	set(XERCES_USE_TRANSCODER_WINDOWS YES)
	set(XERCES_PLATFORM_EXPORT "__declspec( dllexport )")
	set(XERCES_PLATFORM_IMPORT "__declspec( dllimport )")
else()
	if(APPLE AND NOT IOS)
		set(XERCES_USE_TRANSCODER_MACOSUNICODECONVERTER YES)
	else()
		set(XERCES_USE_TRANSCODER_ICONV YES)
	endif()
	set(XERCES_USE_FILEMGR_POSIX YES)
endif()
set(XERCES_USE_MSGLOADER_INMEMORY YES)

configure_file(config.h.cmake config.h)
configure_file(src/xercesc/util/Xerces_autoconf_config.hpp.cmake xercesc/util/Xerces_autoconf_config.hpp)
add_definitions("-DHAVE_CONFIG_H")

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"src"
)

add_subdirectory(src)
