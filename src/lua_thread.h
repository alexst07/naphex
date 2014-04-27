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

#ifndef LUA_THREAD_H
#define LUA_THREAD_H

extern "C" {
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nph_thread.h"

int luaopen_threadlib(lua_State *L);

#endif // LUA_THREAD_H
