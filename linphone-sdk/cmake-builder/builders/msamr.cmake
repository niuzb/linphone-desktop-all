############################################################################
# msamr.cmake
# Copyright (C) 2014-2018  Belledonne Communications, Grenoble France
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

lcb_git_repository("https://gitlab.linphone.org/BC/public/msamr.git")
lcb_git_tag_latest("master")
lcb_git_tag("1.1.2")
lcb_external_source_paths("msamr")
lcb_groupable(YES)
lcb_sanitizable(YES)
lcb_package_source(YES)
lcb_plugin(YES)

lcb_dependencies("ms2")
if(ENABLE_AMRWB)
	lcb_dependencies("voamrwbenc")
endif()
lcb_dependencies("opencoreamr")

lcb_cmake_options(
	"-DENABLE_NARROWBAND=${ENABLE_AMRNB}"
	"-DENABLE_WIDEBAND=${ENABLE_AMRWB}"
)
