############################################################################
# CMakeLists.txt
# Copyright (C) 2016-2018  Belledonne Communications, Grenoble France
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

project(BV16 VERSION 2.1 LANGUAGES C)

include(GNUInstallDirs)

include_directories(${CMAKE_BINARY_DIR} ./bv16)
include_directories(${CMAKE_BINARY_DIR} ./bvcommon)

option(ENABLE_SHARED "Build shared library." YES)
option(ENABLE_STATIC "Build static library." YES)
option(ENABLE_TOOLS "Build tools." NO)


find_library(LIBM NAMES m)


set(BV16_SOURCE_FILES
	bv16/bitpack.c
	bv16/coarptch.c
	bv16/decoder.c
	bv16/encoder.c
	bv16/excdec.c
	bv16/excquan.c
	bv16/fineptch.c
	bv16/gaindec.c
	bv16/gainquan.c
	bv16/levelest.c
	bv16/lspdec.c
	bv16/lspquan.c
	bv16/plc.c
	bv16/postfilt.c
	bv16/ptquan.c
	bv16/tables.c
	bvcommon/a2lsp.c
	bvcommon/allpole.c
	bvcommon/allzero.c
	bvcommon/autocor.c
	bvcommon/cmtables.c
	bvcommon/levdur.c
	bvcommon/lsp2a.c
	bvcommon/ptdec.c
	bvcommon/stblchck.c
	bvcommon/stblzlsp.c
	bvcommon/utility.c
)
set(BROADVOICE16_SOURCE_FILES
	bv16/bv.c
	bv16/g192.c
)

# Levinson method conflicts with AMRWB, so renaming this one
add_definitions("-DLevinson=bv16_Levinson")

add_definitions("-DWMOPS=0" "-DG192BITSTREAM=0")


if(MSVC)
	list(APPEND BV16_SOURCE_FILES bv16.def)
	set_source_files_properties(bv16/g192.c PROPERTIES COMPILE_FLAGS "/wd4267") # Disable "possible loss of data" warning
endif(MSVC)

set(LIBS )
if(LIBM)
	list(APPEND LIBS ${LIBM})
endif()

if(ENABLE_STATIC)
	add_library(bv16-static STATIC ${BV16_SOURCE_FILES})
	set_target_properties(bv16-static PROPERTIES OUTPUT_NAME bv16)
	target_link_libraries(bv16-static ${LIBS})
	install(TARGETS bv16-static
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif()
if(ENABLE_SHARED)
	add_library(bv16 SHARED ${BV16_SOURCE_FILES})
	target_link_libraries(bv16 ${LIBS})
	install(TARGETS bv16
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
	if(MSVC)
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/bv16.pdb
				DESTINATION bin
				PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
			)
		endif()
	endif()
endif()

if(ENABLE_TOOLS)
	add_executable(BroadVoice16 ${BROADVOICE16_SOURCE_FILES})
	if(ENABLE_SHARED)
		target_link_libraries(BroadVoice16 bv16)
	else()
		target_link_libraries(BroadVoice16 bv16-static)
	endif()
	if (NOT IOS)
		install(TARGETS BroadVoice16
			RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
			LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)
	endif()
endif()

file(GLOB BV16_HEADER_FILES "bv16/*.h")
install(FILES ${BV16_HEADER_FILES}
	DESTINATION include/bv16-floatingpoint/bv16
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

file(GLOB BV16_COMMON_HEADER_FILES "bvcommon/*.h")
install(FILES ${BV16_COMMON_HEADER_FILES}
	DESTINATION include/bv16-floatingpoint/bvcommon
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

add_subdirectory(build)
