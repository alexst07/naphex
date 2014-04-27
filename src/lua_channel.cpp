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

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <string>
#include "channel.h"

#define CHANNEL        "channel"

static int lf_channel_create(lua_State *L) {
  const char* str_name = luaL_checkstring(L, 1);
  luaL_argcheck(L, str_name != NULL, 1, "'Invalid string name argument");

  const char* n = luaL_checkstring(L, 2);

  if (channel::exist(str_name)) {
    lua_pushboolean(L, 0);
  }
  else {
    channel::insert(str_name, n);
    lua_pushboolean(L, 1);
  }

  return 1;
}

static int lf_channel_get(lua_State *L) {
  const char* str_name = luaL_checkstring(L, 1);
  luaL_argcheck(L, str_name != NULL, 1, "'Invalid string name argument");

  if (channel::exist(str_name)) {
    std::string sn;
    sn = channel::get(str_name);
    const char *s = sn.c_str();
    lua_pushstring(L, s);
    lua_pushboolean(L, 1);
    return 2;
  }
  else {
    lua_pushboolean(L, 0);
    return 1;
  }
}

static int lf_channel_set(lua_State *L) {
  const char* str_name = luaL_checkstring(L, 1);
  luaL_argcheck(L, str_name != NULL, 1, "'Invalid string name argument");

  const char *sn = luaL_checkstring(L, 2);

  if (channel::exist(str_name)) {
    bool result = channel::set(str_name, sn);
    if (result)
      lua_pushboolean(L, 1);
    else
      lua_pushboolean(L, 0);

    return 1;
  }
  else {
    lua_pushboolean(L, 0);
    return 1;
  }
}

static int lf_channel_exist(lua_State *L) {
  const char* str_name = luaL_checkstring(L, 1);
  luaL_argcheck(L, str_name != NULL, 1, "'Invalid string name argument");

  if (channel::exist(str_name)) {
    lua_pushboolean(L, 1);
  }
  else {
    lua_pushboolean(L, 0);
  }

  return 1;
}

static int lf_channel_erase(lua_State *L) {
  const char* str_name = luaL_checkstring(L, 1);
  luaL_argcheck(L, str_name != NULL, 1, "'Invalid string name argument");

  if (channel::exist(str_name)) {
    bool result = channel::erase(str_name);
    if (result)
      lua_pushboolean(L, 1);
    else
      lua_pushboolean(L, 0);
  }
  else {
    lua_pushboolean(L, 0);
  }

  return 1;
}

int luaopen_channellib(lua_State *L) {
  static const luaL_Reg Obj_lib[] = {
    { "create",         &lf_channel_create },
    { "set",            &lf_channel_set },
    { "get",            &lf_channel_get },
    { "exist",          &lf_channel_exist },
    { "erase",          &lf_channel_erase },
    { NULL,             NULL }
  };

  luaL_newlib(L, Obj_lib);
  return 1;
}
