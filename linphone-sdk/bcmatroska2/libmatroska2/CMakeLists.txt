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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
############################################################################

set(MATROSKA2_SOURCE_FILES
	matroskamain.c
	matroskablock.c
	matroska_sem.c
)

set(LIBS )
if(WIN32 AND NOT CMAKE_SYSTEM_NAME STREQUAL "WindowsPhone" AND NOT CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	list(APPEND LIBS Winmm)
endif()

string(REPLACE ";" " " LINK_FLAGS_STR "${LINK_FLAGS}")
if(ENABLE_STATIC)
	add_library(bcmatroska2 STATIC $<TARGET_OBJECTS:corec-objects> $<TARGET_OBJECTS:ebml2-objects> ${MATROSKA2_SOURCE_FILES})
	set_target_properties(bcmatroska2 PROPERTIES OUTPUT_NAME bcmatroska2)
	set_target_properties(bcmatroska2 PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
	if(NOT "${LINK_FLAGS_STR}" STREQUAL "")
		set_target_properties(bcmatroska2 PROPERTIES LINK_FLAGS "${LINK_FLAGS_STR}")
	endif()
	target_include_directories(bcmatroska2 PRIVATE ../libebml2 .)
	if(LIBS)
		target_link_libraries(bcmatroska2 ${LIBS})
	endif()
	install(TARGETS bcmatroska2 EXPORT BcMatroska2Targets
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif()
if(ENABLE_SHARED)
	add_library(bcmatroska2 SHARED $<TARGET_OBJECTS:corec-objects> $<TARGET_OBJECTS:ebml2-objects> ${MATROSKA2_SOURCE_FILES})
	set_target_properties(bcmatroska2 PROPERTIES VERSION 0.0.0 SOVERSION 0)
	set_target_properties(bcmatroska2 PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
	if(NOT "${LINK_FLAGS_STR}" STREQUAL "")
		set_target_properties(bcmatroska2 PROPERTIES LINK_FLAGS "${LINK_FLAGS_STR}")
	endif()
	target_include_directories(bcmatroska2 PRIVATE ../libebml2 .)
	if(LIBS)
		target_link_libraries(bcmatroska2 ${LIBS})
	endif()
	if(MSVC)
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/bcmatroska2.pdb
				DESTINATION ${CMAKE_INSTALL_BINDIR}
				PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
			)
		endif()
	endif()
	install(TARGETS bcmatroska2 EXPORT BcMatroska2Targets
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif()

install(DIRECTORY matroska
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
