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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monitor.h"
#include "debug.h"
#include "nph_device.h"

#define MONITOR "Monitor"

static monitor* push_monitor(lua_State *L, monitor mon) {
  // create a userdatum to store monitor address
  monitor **pmon = reinterpret_cast<monitor **>(
                lua_newuserdata(L, sizeof(monitor)));
  *pmon = new monitor(mon);

  luaL_getmetatable(L, MONITOR);
  lua_setmetatable(L, -2);

  return *pmon;
}

static monitor* chk_monitor(lua_State *L, int index) {
  monitor *mon;
  luaL_checktype(L, index, LUA_TUSERDATA);
  mon = *reinterpret_cast<monitor **>(luaL_checkudata(L, index, MONITOR));
  if (mon == NULL)
    luaL_error(L, "Argument error in Monitor");

  return mon;
}

static int lf_mon_new(lua_State *L) {
  int argc = lua_gettop(L);
  Debug(5) << "Number of param: " << argc;

  if (argc != 1)
    luaL_error(L, "Monitor expects 1 argument");

  const char* str_device = luaL_checkstring(L, 1);

  push_monitor(L, monitor(str_device));

  luaL_getmetatable(L, MONITOR);
  lua_setmetatable(L, -2);
  Debug(5) << "Object created!";
  return 1;
}

static int lf_mon_main_func_set(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);

  luaL_argcheck(L, mon != NULL, 1, "'Monitor' expected");

  const char* str_func = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_func != NULL, 2, "Invalid string argument");
  mon->set_main_func(str_func);

  return 0;
}

static int lf_mon_start_set(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);

  luaL_argcheck(L, mon != NULL, 1, "'Monitor' expected");

  const char* str_start = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_start != NULL, 2, "Invalid string argument");
  mon->set_func_start(str_start);

  return 0;
}

static int lf_mon_end_set(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);

  luaL_argcheck(L, mon != NULL, 1, "'Monitor' expected");

  const char* str_end = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_end != NULL, 2, "Invalid string argument");
  mon->set_func_end(str_end);

  return 0;
}

static int lf_mon_lfile_set(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);

  luaL_argcheck(L, mon != NULL, 1, "'Monitor' expected");

  const char* str_lfile = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_lfile != NULL, 2, "Invalid string argument");
  mon->set_lua_file(str_lfile);

  return 0;
}

static int lf_mon_run(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  mon->run();
  return 0;
}

static int lf_mon_pause(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  mon->pause();
  return 0;
}

static int lf_mon_break(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  mon->exit();
  return 0;
}

static int lf_mon_main_func_get(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  luaL_argcheck(L, mon != NULL, 1, "'Monitor' expected");

  lua_pushstring(L, mon->get_main_func());
  return 1;
}

static int lf_mon_start_get(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  luaL_argcheck(L, mon != NULL, 1, "'Monitor' expected");

  lua_pushstring(L, mon->get_func_start());
  return 1;
}

static int lf_mon_end_get(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  luaL_argcheck(L, mon != NULL, 1, "'Monitor' expected");

  lua_pushstring(L, mon->get_func_end());
  return 1;
}

static int lf_mon_lfile_get(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  luaL_argcheck(L, mon != NULL, 1, "'Monitor' expected");

  lua_pushstring(L, mon->get_lua_file());
  return 1;
}

static int lf_mon_dev_direction(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  int d = luaL_checkint(L, 2);

  if (!mon->setdirection(d))
    luaL_error(L, "Can't set direction to device");

  return 0;
}

static int lf_mon_dev_promisc(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  int p = luaL_checkint(L, 2);

  if (!mon->setpromisc(p))
    luaL_error(L, "Can't set to promisc mode");

  return 0;
}

static int lf_mon_dev_rfmonitor(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  int r = luaL_checkint(L, 2);

  if (!mon->setrfmonitor(r))
    luaL_error(L, "Can't set rf monitor");

  return 0;
}

static int lf_mon_dev_timeout(lua_State *L) {
  monitor *mon = chk_monitor(L, 1);
  int t = luaL_checkint(L, 2);

  if (!mon->settimeout(t))
    luaL_error(L, "Can't set timeout");

  return 0;
}

static int lf_mon_gc(lua_State *L) {
  Debug(5) << "gc_start";
  monitor *mon = *reinterpret_cast<monitor **>(lua_touserdata(L, 1));
  luaL_argcheck(L, mon != NULL, 1, "'Monitor' expected");

  if (mon)
    delete mon;

  Debug(5) << "gc_end";
  return 0;
}

int luaopen_monlib(lua_State *L) {
  static const luaL_Reg Obj_lib[] = {
    { "set_fmain", &lf_mon_main_func_set },
    { "set_fstart", &lf_mon_start_set },
    { "set_fend", &lf_mon_end_set },
    { "set_file", &lf_mon_lfile_set },
    { "get_fmain", &lf_mon_main_func_get },
    { "get_fstart", &lf_mon_start_get },
    { "get_file", &lf_mon_lfile_get },
    { "get_fend", &lf_mon_end_get },
    { "dev_promisc", &lf_mon_dev_promisc },
    { "dev_direction", &lf_mon_dev_direction },
    { "dev_rfmonitor", &lf_mon_dev_rfmonitor },
    { "dev_timeout", &lf_mon_dev_timeout },
    { "run", &lf_mon_run },
    { "pause", &lf_mon_pause },
    { "exit", &lf_mon_break },
    { NULL, NULL }
  };

  static const luaL_Reg LuaLib_Mon[] = {
    { "new", &lf_mon_new },
    { NULL, NULL }
  };

  luaL_newlib(L, LuaLib_Mon);

  // Stack: MyLib
  luaL_newmetatable(L, MONITOR);        // Stack: MyLib meta
  luaL_newlib(L, Obj_lib);
  lua_setfield(L, -2, "__index");       // Stack: MyLib meta

  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, lf_mon_gc);      // Stack: MyLib meta "__gc" fptr
  lua_settable(L, -3);                  // Stack: MyLib meta
  lua_pop(L, 1);                        // Stack: MyLib

  return 1;
}
