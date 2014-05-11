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
## Include required files
##
include (cmake/NaphexUtils.cmake)

##
## Set the variable CMAKE_MODULE_PATH to call find modules in the CMakeLists
##
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

##
## The version number.
##
set (NAPHEX_VERSION_MAJOR 0)
set (NAPHEX_VERSION_MINOR 0)
set (NAPHEX_VERSION_RELEASE 1)
set (NAPHEX_VERSION "${naphex_VERSION_MAJOR}.${naphex_VERSION_MINOR}.${naphex_VERSION_RELEASE}")

##
## Set naphex install path
##
set (NAPHEX_INSTALL_DEST bin)

##
## Set the lib install path
##
set (NAPHEX_INSTALL_LIB_PATH lib/naphex)

##
## Set config install path
##
set (NAPHEX_INSTALL_CONF_PATH etc/naphex)

##
## configure a header file to pass some of the CMake settings
## to the source code
##
configure_file (
  "${PROJECT_SOURCE_DIR}/src/define.h.in"
  "${PROJECT_BINARY_DIR}/include/define.h"
  )

##
## configure the configuration file
##
configure_file (
  "${PROJECT_SOURCE_DIR}/naphex.conf.in"
  "${PROJECT_BINARY_DIR}/naphex.conf"
  )

##
## Set debug mode if is running as debug mode
##
if( CMAKE_BUILD_TYPE STREQUAL "Release") 
  set (NAPHEX_DEBUG "#define NDEBUG")
endif ()

##
## Set extension dir
##
set (NAPHEX_EXT_DIR "${CMAKE_INSTALL_PREFIX}/${NAPHEX_INSTALL_LIB_PATH}")

##
## add the binary tree to the search path for include files
##
include_directories ("${PROJECT_BINARY_DIR}/include")

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
              ${CMAKE_CURRENT_SOURCE_DIR}/src/config_imp.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

##
## find pcap packet
##
find_package(PCAP REQUIRED)

if (PCAP_FOUND)
  include_directories (${PCAP_INCLUDE_DIR})
  target_link_libraries (naphex  ${PCAP_LIBRARY})
endif()

##
## find Lua
##
find_package(Lua REQUIRED)

if (LUA_FOUND)
  include_directories (${LUA_INCLUDE_DIR})
  target_link_libraries (naphex  ${LUA_LIBRARIES})
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
  target_link_libraries (naphex dl)
  target_link_libraries (naphex m)
endif()

##
## Set the flags for g++ compiler
##
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS   "-std=c++11")
  set ( CMAKE_CXX_FLAGS_DEBUG  "-Wall -g" )
endif()

##
## Set the install targets
##
install (TARGETS naphex RUNTIME DESTINATION ${NAPHEX_INSTALL_DEST})
install (FILES "${PROJECT_BINARY_DIR}/naphex.conf" DESTINATION
         ${NAPHEX_INSTALL_CONF_PATH})

add_subdirectory (src)

## ----------------------------------------------------------------------------
## Summary:
## ----------------------------------------------------------------------------
status("")
status("General configuration for Naphex ${NAPHEX_VERSION} =======================")

##
## ========================== build platform ==========================
##
status("")
status(" Platform:")
status(" Host:" ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION} ${CMAKE_HOST_SYSTEM_PROCESSOR})
if(CMAKE_CROSSCOMPILING)
  status(" Target:" ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR})
endif()
status(" CMake:" ${CMAKE_VERSION})
status(" CMake generator:" ${CMAKE_GENERATOR})
status(" CMake build tool:" ${CMAKE_BUILD_TOOL})
if(MSVC)
  status(" MSVC:" ${MSVC_VERSION})
endif()
if(CMAKE_GENERATOR MATCHES Xcode)
  status(" Xcode:" ${XCODE_VERSION})
endif()
if(NOT CMAKE_GENERATOR MATCHES "Xcode|Visual Studio")
  status(" Configuration:" ${CMAKE_BUILD_TYPE})
endif()

##
## ========================== C/C++ options ==========================
##
if(CMAKE_CXX_COMPILER_VERSION)
  set(NAPHEX_COMPILER_STR "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1} (ver ${CMAKE_CXX_COMPILER_VERSION})")
elseif(CMAKE_COMPILER_IS_CLANGCXX)
  set(NAPHEX_COMPILER_STR "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1} (ver ${CMAKE_CLANG_REGEX_VERSION})")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  set(NAPHEX_COMPILER_STR "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1} (ver ${CMAKE_GCC_REGEX_VERSION})")
else()
  set(NAPHEX_COMPILER_STR "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1}")
endif()
string(STRIP "${NAPHEX_COMPILER_STR}" NAPHEXCOMPILER_STR)

status("")
status(" C/C++:")
status(" Built as dynamic libs?:" BUILD_SHARED_LIBS THEN YES ELSE NO)
status(" C++ Compiler:" ${NAPHEX_COMPILER_STR})
status(" C++ flags (Release):" ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE})
status(" C++ flags (Debug):" ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG})
status(" C Compiler:" ${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_ARG1})
status(" C flags (Release):" ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE})
status(" C flags (Debug):" ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG})
if(WIN32)
  status(" Linker flags (Release):" ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
  status(" Linker flags (Debug):" ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
else()
  status(" Linker flags (Release):" ${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
  status(" Linker flags (Debug):" ${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
endif()
status(" Precompiled headers:" PCHSupport_FOUND AND ENABLE_PRECOMPILED_HEADERS THEN YES ELSE NO)
status("")