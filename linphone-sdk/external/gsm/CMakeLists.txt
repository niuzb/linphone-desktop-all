############################################################################
# CMakeLists.txt
# Copyright (C) 2014  Belledonne Communications, Grenoble France
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

cmake_minimum_required(VERSION 3.1)
project(GSM C)


option(ENABLE_STATIC "Build static library (default is shared library)." OFF)


include_directories(
	inc/
)

set(GSM_SOURCE_FILES
	src/add.c
	src/code.c
	src/debug.c
	src/decode.c
	src/long_term.c
	src/lpc.c
	src/preprocess.c
	src/rpe.c
	src/gsm_destroy.c
	src/gsm_decode.c
	src/gsm_encode.c
	src/gsm_explode.c
	src/gsm_implode.c
	src/gsm_create.c
	src/gsm_print.c
	src/gsm_option.c
	src/short_term.c
	src/table.c
)
if(MSVC)
	list(APPEND GSM_SOURCE_FILES gsm.def)
endif(MSVC)

if(ENABLE_STATIC)
	add_library(gsm STATIC ${GSM_SOURCE_FILES})
else()
	add_library(gsm SHARED ${GSM_SOURCE_FILES})
	if(MSVC)
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/gsm.pdb
				DESTINATION bin
				PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
			)
		endif()
	endif()
endif()

include(GNUInstallDirs)

target_compile_options(gsm PRIVATE "-w")

install(TARGETS gsm
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)


file(GLOB HEADER_FILES "inc/gsm.h")

install(FILES ${HEADER_FILES}
	DESTINATION include/gsm
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
