# 
#  This file is part of the Naphex project
#  Copyright (C) 2014  Alex Silva Torres <alexst07@gmail.com>
# 
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
# --------------------------------------------------------------------------
# - Try to find PCAP
# Once done, this will define
#
#  PCAP_FOUND - system has libpcap
#  PCAP_INCLUDE_DIR - the libpcap include directories
#  PCAP_LIBRARy - link these to use libpcap

find_path(PCAP_ROOT_DIR
    NAMES include/pcap.h
)

find_path(PCAP_INCLUDE_DIR
    NAMES pcap.h
    HINTS ${PCAP_ROOT_DIR}/include
)

find_library(PCAP_LIBRARY
    NAMES pcap
    HINTS ${PCAP_ROOT_DIR}/lib
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LUA_FOUND to TRUE if
# all listed variables are TRUE
find_package_handle_standard_args(PCAP REQUIRED_VARS
                                  PCAP_LIBRARY
                                  PCAP_INCLUDE_DIR)

mark_as_advanced(PCAP_ROOT_DIR
                 PCAP_INCLUDE_DIR
                 PCAP_LIBRARY)