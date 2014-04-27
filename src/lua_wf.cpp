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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wait_filter.h"
#include "debug.h"
#include "nph_device.h"

#define WAITFILTER      "wait_filter"

static wait_filter* push_wf(lua_State *L, wait_filter wf) {
  // create a userdatum to store wait_filter address
  wait_filter **pwf = reinterpret_cast<wait_filter **>(
                lua_newuserdata(L, sizeof(wait_filter)));
  *pwf = new wait_filter(wf);

  luaL_getmetatable(L, WAITFILTER);
  lua_setmetatable(L, -2);

  return *pwf;
}

static wait_filter* chk_wf(lua_State *L, int index) {
  wait_filter *wf;
  luaL_checktype(L, index, LUA_TUSERDATA);
  wf = *reinterpret_cast<wait_filter **>(luaL_checkudata(L, index, WAITFILTER));
  if (wf == NULL)
    luaL_error(L, "Argument error in WaitFilter");

  return wf;
}

static int lf_wf_new(lua_State *L) {
  int argc = lua_gettop(L);
  Debug(5) << "Number of param: " << argc;

  if (argc != 1)
    luaL_error(L, "WaitFilter expects 1 argument");

  const char* str_device = luaL_checkstring(L, 1);

  push_wf(L, wait_filter(str_device));

  luaL_getmetatable(L, WAITFILTER);
  lua_setmetatable(L, -2);
  Debug(5) << "Object created!";
  return 1;
}

static int lf_wf_filter_set(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);

  luaL_argcheck(L, wf != NULL, 1, "'WaitFilter' expected");

  const char* str_filter = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_filter != NULL, 2, "Invalid string argument");
  wf->set_func_filter(str_filter);

  return 0;
}

static int lf_wf_action_set(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);

  luaL_argcheck(L, wf != NULL, 1, "'WaitFilter' expected");

  const char* str_action = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_action != NULL, 2, "Invalid string argument");
  wf->set_func_action(str_action);

  return 0;
}

static int lf_wf_lfile_set(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);

  luaL_argcheck(L, wf != NULL, 1, "'WaitFilter' expected");

  const char* str_lfile = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_lfile != NULL, 2, "Invalid string argument");
  wf->set_lua_file(str_lfile);

  return 0;
}

static int lf_wf_maxtime_set(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);

  luaL_argcheck(L, wf != NULL, 1, "'WaitFilter' expected");

  int t = luaL_checkinteger(L, 2);
  Debug(5) << "MaxTime";
  wf->set_maxtime(t);

  return 0;
}

static int lf_wf_filter_get(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);
  luaL_argcheck(L, wf != NULL, 1, "'WaitFilter' expected");

  lua_pushstring(L, wf->get_func_filter());
  return 1;
}

static int lf_wf_action_get(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);
  luaL_argcheck(L, wf != NULL, 1, "'WaitFilter' expected");

  lua_pushstring(L, wf->get_func_action());
  return 1;
}

static int lf_wf_lfile_get(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);
  luaL_argcheck(L, wf != NULL, 1, "'Monitor' expected");

  lua_pushstring(L, wf->get_lua_file());
  return 1;
}

static int lf_wf_maxtime_get(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);
  luaL_argcheck(L, wf != NULL, 1, "'WaitFilter' expected");

  lua_pushinteger(L, wf->get_maxtime());
  return 1;
}

static int lf_wf_wait(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);
  wf->wait();
  return 0;
}

static int lf_wf_dev_direction(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);
  int d = luaL_checkint(L, 2);

  if (!wf->set_dev_direction(d))
    luaL_error(L, "Can't set direction to device");

  return 0;
}

static int lf_wf_dev_promisc(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);
  int p = luaL_checkint(L, 2);

  if (!wf->set_dev_promisc(p))
    luaL_error(L, "Can't set to promisc mode");

  return 0;
}

static int lf_wf_dev_rfmonitor(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);
  int r = luaL_checkint(L, 2);

  if (!wf->set_dev_rfmonitor(r))
    luaL_error(L, "Can't set rf monitor");

  return 0;
}

static int lf_wf_dev_timeout(lua_State *L) {
  wait_filter *wf = chk_wf(L, 1);
  int t = luaL_checkint(L, 2);

  if (!wf->set_dev_timeout(t))
    luaL_error(L, "Can't set timeout");

  return 0;
}

static int lf_wf_gc(lua_State *L) {
  Debug(5) << "gc_start";
  wait_filter **wf = reinterpret_cast<wait_filter **>(lua_touserdata(L, 1));
  luaL_argcheck(L, *wf != NULL, 1, "'WaitFilter' expected");

  delete *wf;
  Debug(5) << "gc_end";
  return 0;
}

int luaopen_wflib(lua_State *L) {
  static const luaL_Reg Obj_lib[] = {
    { "set_action", &lf_wf_action_set },
    { "set_filter", &lf_wf_filter_set },
    { "set_file", &lf_wf_lfile_set },
    { "set_maxtime", &lf_wf_maxtime_set },
    { "get_filter", &lf_wf_filter_get },
    { "get_action", &lf_wf_action_get },
    { "get_file", &lf_wf_lfile_get },
    { "get_maxtime", &lf_wf_maxtime_get },
    { "dev_promisc", &lf_wf_dev_promisc },
    { "dev_direction", &lf_wf_dev_direction },
    { "dev_rfmonitor", &lf_wf_dev_rfmonitor },
    { "dev_timeout", &lf_wf_dev_timeout },
    { "wait", &lf_wf_wait },
    { NULL, NULL }
  };

  static const luaL_Reg LuaLib_WF[] = {
    { "new", &lf_wf_new },
    { NULL, NULL }
  };

  luaL_newlib(L, LuaLib_WF);

  // Stack: MyLib
  luaL_newmetatable(L, WAITFILTER);        // Stack: MyLib meta
  luaL_newlib(L, Obj_lib);
  lua_setfield(L, -2, "__index");       // Stack: MyLib meta

  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, lf_wf_gc);      // Stack: MyLib meta "__gc" fptr
  lua_settable(L, -3);                  // Stack: MyLib meta
  lua_pop(L, 1);                        // Stack: MyLib

  return 1;
}
