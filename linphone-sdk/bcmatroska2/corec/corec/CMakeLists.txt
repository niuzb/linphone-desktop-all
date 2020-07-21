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

set(COREC_SOURCES
	array/array.c
	helpers/file/bufstream.c
	helpers/file/memstream.c
	helpers/file/streams.c
	helpers/file/tools.c
	helpers/md5/md5.c
	helpers/parser/parser2.c
	helpers/parser/strtab.c
	helpers/parser/strtypes.c
	helpers/parser/dataheap.c
	helpers/parser/buffer.c
	helpers/parser/hotkey.c
	helpers/parser/nodelookup.c
	helpers/parser/urlpart.c
	node/node.c
	node/nodetree.c
	str/str.c
)

if(WIN32)
	list(APPEND COREC_SOURCES
		helpers/charconvert/charconvert_win32.c
		helpers/date/date_win32.c
		helpers/file/file_win32.c
		helpers/system/ccsystem_win32.c
		multithread/multithread_win32.c
		str/str_win32.c
	)
else()
	list(APPEND COREC_SOURCES
		helpers/date/date_libc.c
		helpers/file/file_libc.c
		multithread/multithread_pthread.c
	)
	if (NOT IOS) 
		list(APPEND COREC_SOURCES
			helpers/system/ccsystem_posix.c
		)	
	endif()
	if(APPLE)
		list(APPEND COREC_SOURCES
			helpers/charconvert/charconvert_osx.c
			str/str_osx.c
		)
	elseif(ANDROID)
		list(APPEND COREC_SOURCES
			helpers/charconvert/charconvert_utf8.c
			str/str_linux.c
		)
	elseif(QNX)
		list(APPEND COREC_SOURCES
                        helpers/charconvert/charconvert_utf8.c
                        str/str_utf8.c
                )
	else()
		list(APPEND COREC_SOURCES
			helpers/charconvert/charconvert_linux.c
			str/str_linux.c
		)
	endif()
endif()

if(CONFIG_STDIO)
	list(APPEND COREC_SOURCES
		helpers/file/stream_stdio.c
	)
endif()

add_library(corec-objects OBJECT ${COREC_SOURCES})
set_target_properties(corec-objects PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

install(FILES
	banned.h
	confhelper.h
	corec.h
	err.h
	helper.h
	memalloc.h
	memheap.h
	portab.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec
)

install(FILES array/array.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/array)
install(FILES helpers/charconvert/charconvert.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/helpers/charconvert)
install(FILES helpers/date/date.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/helpers/date)
install(FILES
	helpers/file/file.h
	helpers/file/streams.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/helpers/file
)
install(FILES helpers/md5/md5.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/helpers/md5)
install(FILES
	helpers/parser/buffer.h
	helpers/parser/dataheap.h
	helpers/parser/hotkey.h
	helpers/parser/nodelookup.h
	helpers/parser/parser.h
	helpers/parser/strtab.h
	helpers/parser/strtypes.h
	helpers/parser/urlpart.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/helpers/parser
)
install(FILES helpers/system/ccsystem.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/helpers/system)
install(FILES multithread/multithread.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/multithread)
install(FILES
	node/node.h
	node/node_internal.h
	node/nodebase.h
	node/nodetools.h
	node/nodetree.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/node
)
install(FILES str/str.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/corec/str)
