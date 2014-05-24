/*
 * This file is part of the Naphex project
 * Copyright (C) 2014  Alex Silva Torres <alexst07@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file lualibs.h
 *
 * In this file is declared the functions to create lua libs.
 * Lua libs can be called by other class that execute lua scripts
 * to allow the executions of lua functions
 */

#ifndef NAPHEX_LUALIBS_H
#define NAPHEX_LUALIBS_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

/**
 * Create the lua lib to channel
 */
int luaopen_channellib(lua_State *L);

/**
 * Create the lua lib to monitor
 */
int luaopen_monlib(lua_State *L);

/**
 * Create the lua lib to packet
 */
int luaopen_packetlib(lua_State *L);

/**
 * Create the lua lib to thread
 */
int luaopen_threadlib(lua_State *L);

/**
 * Create the lua lib to wait filter
 */
int luaopen_wflib(lua_State *L);

/**
 * Create the lua lib to online device
 */
int luaopen_on_devlib(lua_State *L);

/**
 * Create the lua lib to offline device
 */
int luaopen_off_devlib(lua_State *L);

/**
 * Create the lua function to list all devices
 */
int luaopen_ldevfunc(lua_State *L);

#endif  // NAPHEX_LUALIBS_H
