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

cmake_minimum_required(VERSION 3.0)
project(mssilk VERSION 1.1.1 LANGUAGES C)


option(ENABLE_SHARED "Build shared library." YES)
option(ENABLE_STATIC "Build static library." YES)


include(GNUInstallDirs)

if(NOT CMAKE_INSTALL_RPATH AND CMAKE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR})
	message(STATUS "Setting install rpath to ${CMAKE_INSTALL_RPATH}")
endif()

find_package(Mediastreamer2 CONFIG REQUIRED)
find_package(bctoolbox CONFIG REQUIRED)
find_package(ortp CONFIG REQUIRED)

set(FLAVOUR "FIX")
file(GLOB SILK_SOURCE_FILES "sdk/SILK_SDK_SRC_v1.0.9/SILK_SDK_SRC_${FLAVOUR}_v1.0.9/src/*.c")

# turn off warnings for the sdk compilation since we won't fix them
set_source_files_properties(${SILK_SOURCE_FILES} PROPERTIES COMPILE_FLAGS -w)

include_directories(
	${MEDIASTREAMER2_INCLUDE_DIRS}
	sdk/SILK_SDK_SRC_v1.0.9/SILK_SDK_SRC_${FLAVOUR}_v1.0.9/interface
)

add_definitions(-DVERSION=\"${PROJECT_VERSION}\")


set(LIBS ${MEDIASTREAMER2_LIBRARIES} ${ORTP_LIBRARIES} ${BCTOOLBOX_CORE_LIBRARIES})

set(SOURCE_FILES
	${SILK_SOURCE_FILES}
	silk_dec.c
	silk_enc.c
)

set(MS2_PLUGINS_DIR "${MEDIASTREAMER2_PLUGINS_LOCATION}")

if(ENABLE_STATIC)
	add_library(mssilk-static STATIC ${SOURCE_FILES})
	set_target_properties(mssilk-static PROPERTIES OUTPUT_NAME mssilk)
	target_link_libraries(mssilk-static ${LIBS})
	install(TARGETS mssilk-static
		ARCHIVE DESTINATION "${MS2_PLUGINS_DIR}"
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif()
if(ENABLE_SHARED)
	if(NOT IOS)
		add_library(mssilk MODULE ${SOURCE_FILES})
	else()
		add_library(mssilk SHARED ${SOURCE_FILES})
	endif()
	target_link_libraries(mssilk ${LIBS})
	if(IOS)
		set(MIN_OS ${LINPHONE_IOS_DEPLOYMENT_TARGET})
		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/osx/")
		set_target_properties(mssilk PROPERTIES
			FRAMEWORK TRUE
			MACOSX_FRAMEWORK_IDENTIFIER org.linphone.mssilk
  			MACOSX_FRAMEWORK_INFO_PLIST Info.plist.in
		) 
	endif()
	if(MSVC)
		if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/libmssilk.pdb
				DESTINATION ${CMAKE_INSTALL_BINDIR}
				PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
			)
		endif()
		set_target_properties(mssilk PROPERTIES PREFIX "lib")
	endif()
	install(TARGETS mssilk
		RUNTIME DESTINATION ${MS2_PLUGINS_DIR}
		LIBRARY DESTINATION ${MS2_PLUGINS_DIR}
		ARCHIVE DESTINATION ${MS2_PLUGINS_DIR}
		FRAMEWORK DESTINATION Frameworks
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif()


# CPack settings
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES
	"^${CMAKE_BINARY_DIR}"
	"/\\\\..+"
)

include(CPack)
