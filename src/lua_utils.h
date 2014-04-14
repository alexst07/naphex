/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#ifndef LUA_UTILS_H
#define LUA_UTILS_H

extern "C" {
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *lua_filename = NULL;

void set_lua_filename(const char *name);

const char *get_lua_filename();

void np_lua_func(lua_State *L, const char *func, const char *args, const char *res, ...);
void lfiler_func(lua_State *L, const char *func, const u_char *packet, u_int len, int *ret);
void laction_func(lua_State *L, const char *func, const u_char *packet, u_int len);


#endif // LUA_UTILS_H
