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

project(naphex)

##
## Minimum requrired cmake version
##
cmake_minimum_required(VERSION 2.6)

##
## Set the variable CMAKE_MODULE_PATH to call find modules in the CMakeLists
##
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

##
## Add the source files
##
add_executable(naphex 
              ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_channel.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/channel.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/args.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_packet.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/packet.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_thread.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/nph_thread.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_wf.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_dev.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_monitor.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/nph_device.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/lua_utils.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/wait_filter.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/off_device.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/on_device.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/monitor.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

##
## find pcap packet
##
find_package(PCAP REQUIRED)

if (PCAP_FOUND)
  include_directories(${PCAP_INCLUDE_DIR})
  target_link_libraries(naphex  ${PCAP_LIBRARY})
endif()

##
## find Lua
##
find_package(Lua REQUIRED)

if (LUA_FOUND)
  include_directories(${LUA_INCLUDE_DIR})
  target_link_libraries(naphex  ${LUA_LIBRARIES})
endif()

##
## Find the specific thread for each plataform
##
find_package (Threads)
target_link_libraries (naphex ${CMAKE_THREAD_LIBS_INIT})

##
## link the libraries used in UNIX-like OS's
##
if (UNIX)
  target_link_libraries(naphex dl)
  target_link_libraries(naphex m)
endif()

##
## Set the flags for g++ compiler
##
if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS   "-std=c++11")
endif()

install(TARGETS naphex RUNTIME DESTINATION bin)

add_subdirectory(src)