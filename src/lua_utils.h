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

#ifndef NAPHEX_LUA_UTILS_H
#define NAPHEX_LUA_UTILS_H

extern "C" {
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>

using std::list;
using std::string;

static const char *lua_filename = NULL;
static const char *ext_dir = NULL;

void set_lua_filename(const char *name);

const char *get_lua_filename();

void np_lua_func(lua_State *L, const char *func, const char *args,
                 const char *res, ...);
void lfiler_func(lua_State *L, const char *func, const u_char *packet,
                 u_int len, int *ret);
void laction_func(lua_State *L, const char *func, const u_char *packet,
                  u_int len);

bool llib_load(list<string> & libs, lua_State *L, const string &path);

bool conf_load(lua_State *L, const string &file);

void get_libs(lua_State *L, string table, list<string> & libs);

bool load_string(lua_State *L, const string &field, string &value);

bool load_int(lua_State *L, const string &field, int &value);

void conf_close(lua_State *L);

#endif  // NAPHEX_LUA_UTILS_H
