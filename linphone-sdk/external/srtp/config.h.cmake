/***************************************************************************
* config.h.cmake
* Copyright (C) 2014  Belledonne Communications, Grenoble France
*
****************************************************************************
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
****************************************************************************/

#cmakedefine ENABLE_DEBUGGING 1
#cmakedefine SRTP_KERNEL 1
#cmakedefine SRTP_KERNEL_LINUX 1
#cmakedefine DEV_URANDOM "@DEV_URANDOM@"
#cmakedefine GENERIC_AESICM 1
#cmakedefine USE_SYSLOG 1
#cmakedefine ERR_REPORTING_STDOUT 1
#cmakedefine USE_ERR_REPORTING_FILE 1
#cmakedefine ERR_REPORTING_FILE "@ERR_REPORTING_FILE@"
#cmakedefine SRTP_GDOI 1

#cmakedefine CPU_CISC 1
#cmakedefine CPU_RISC 1
#cmakedefine HAVE_X86 1
#cmakedefine WORDS_BIGENDIAN 1

#cmakedefine HAVE_STDLIB_H 1
#cmakedefine HAVE_UNISTD_H 1
#cmakedefine HAVE_BYTESWAP_H 1
#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_INTTYPES_H 1
#cmakedefine HAVE_SYS_TYPES_H 1
#cmakedefine HAVE_MACHINE_TYPES_H 1
#cmakedefine HAVE_SYS_INT_TYPES_H 1
#cmakedefine HAVE_SYS_SOCKET_H 1
#cmakedefine HAVE_NETINET_IN_H 1
#cmakedefine HAVE_ARPA_INET_H 1
#cmakedefine HAVE_WINDOWS_H 1
#cmakedefine HAVE_WINSOCK2_H 1
#cmakedefine HAVE_SYSLOG_H 1

#cmakedefine HAVE_INET_ATON 1
#cmakedefine HAVE_USLEEP 1
#cmakedefine HAVE_SIGACTION 1

#cmakedefine HAVE_UINT8_T 1
#cmakedefine HAVE_UINT16_T 1
#cmakedefine HAVE_UINT32_T 1
#cmakedefine HAVE_UINT64_T 1
#cmakedefine SIZEOF_UNSIGNED_LONG @SIZEOF_UNSIGNED_LONG@
#cmakedefine SIZEOF_UNSIGNED_LONG_LONG @SIZEOF_UNSIGNED_LONG_LONG@
#cmakedefine PACKAGE_STRING "libsrtp2 2.2.0-pre"
#cmakedefine PACKAGE_VERSION "2.0.1-pre"

#ifndef __cplusplus
#cmakedefine inline @inline@
#endif
