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

cmake_minimum_required(VERSION 3.1)

project(bcmatroska2
	VERSION 0.23
	LANGUAGES C
)

option(ENABLE_SHARED "Build shared library." YES)
option(ENABLE_STATIC "Build static library." YES)
option(CONFIG_EBML_WRITING "Enable EBML file writing support" ON)
option(CONFIG_EBML_UNICODE "Enable Unicode support" ON)
option(CONFIG_DEBUGCHECKS "Enable internal checks" ON)
option(CONFIG_STDIO "Use stdio.h for writing" ON)
option(CONFIG_FILEPOS_64 "Encode filepos_t on 64bits" ON)
option(CONFIG_DEBUG_LEAKS "Enable internal menory leaks checking support" OFF)

include(GNUInstallDirs)

if(NOT CMAKE_INSTALL_RPATH AND CMAKE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR})
	message(STATUS "Setting install rpath to ${CMAKE_INSTALL_RPATH}")
endif()

configure_file(config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

include_directories(
	${CMAKE_BINARY_DIR}
	corec
)

set(LINK_FLAGS )
if(APPLE)
	list(APPEND LINK_FLAGS "-framework Foundation")
	if(IOS)
		list(APPEND LINK_FLAGS "-framework CoreFoundation")
	endif()
endif()
string(REPLACE ";" " " LINK_FLAGS_STR "${LINK_FLAGS}")

if(WIN32)
	add_definitions(
		"-DEBML2_EXPORTS"
		"-DMATROSKA2_EXPORTS"
		"-DSTR_EXPORTS"
	)
endif()
add_compile_options(-w)

add_subdirectory(corec/corec)
add_subdirectory(libebml2)
add_subdirectory(libmatroska2)

install(FILES ${CMAKE_BINARY_DIR}/config.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec
)

include(CMakePackageConfigHelpers)
set(CONFIG_PACKAGE_LOCATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake")

write_basic_package_version_file(
	${CMAKE_BINARY_DIR}/BcMatroska2ConfigVersion.cmake
	VERSION ${MATROSKA2_VERSION}
	COMPATIBILITY AnyNewerVersion
)

export(EXPORT BcMatroska2Targets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/BcMatroska2Targets.cmake"
)

configure_package_config_file(BcMatroska2Config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/BcMatroska2Config.cmake"
	INSTALL_DESTINATION ${CONFIG_PACKAGE_LOCATION}
	NO_SET_AND_CHECK_MACRO
)

install(EXPORT BcMatroska2Targets
	FILE BcMatroska2Targets.cmake
	DESTINATION ${CONFIG_PACKAGE_LOCATION}
)
install(FILES
	${CMAKE_BINARY_DIR}/BcMatroska2ConfigVersion.cmake
	${CMAKE_BINARY_DIR}/BcMatroska2Config.cmake
	DESTINATION ${CONFIG_PACKAGE_LOCATION}
)

add_subdirectory(build)
