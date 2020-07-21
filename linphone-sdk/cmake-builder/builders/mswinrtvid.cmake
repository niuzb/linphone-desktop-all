############################################################################
# mswinrtvid.cmake
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

lcb_git_repository("https://gitlab.linphone.org/BC/public/mswinrtid.git")
lcb_git_tag_latest("master")
lcb_git_tag("b067bca955dc170b54fffe019a6a7ce86c781c8a")
lcb_external_source_paths("mswinrtvid")
lcb_groupable(YES)
lcb_plugin(YES)

lcb_dependencies("ms2")
