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
#include "nph_thread.h"
#include "debug.h"

#define NPH_THREAD      "thread"

static nph_thread *push_thread(lua_State *L, nph_thread thread) {
  // create a userdatum to store wait_filter address
  nph_thread **pthread = reinterpret_cast<nph_thread **>(
                lua_newuserdata(L, sizeof(nph_thread)));
  *pthread = new nph_thread(thread);

  luaL_getmetatable(L, NPH_THREAD);
  lua_setmetatable(L, -2);

  return *pthread;
}

static nph_thread *chk_thread(lua_State *L, int index) {
  nph_thread *thread;
  luaL_checktype(L, index, LUA_TUSERDATA);
  thread = *reinterpret_cast<nph_thread **>(
           luaL_checkudata(L, index, NPH_THREAD));
  if (thread == NULL)
    luaL_error(L, "Argument error in Thread");

  return thread;
}

static int lf_thread_new(lua_State *L) {
  int argc = lua_gettop(L);
  Debug(5) << "Number of param: " << argc;

  if (argc != 0)
    luaL_error(L, "Thread expects no argument");

  push_thread(L, nph_thread());

  luaL_getmetatable(L, NPH_THREAD);
  lua_setmetatable(L, -2);
  Debug(5) << "Object thread created!";
  return 1;
}

static int lf_thread_func_set(lua_State *L) {
  nph_thread *thread = chk_thread(L, 1);

  luaL_argcheck(L, thread != NULL, 1, "'Thread' expected");

  const char* str_func = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_func != NULL, 2, "Invalid string argument");
  thread->set_lfunc(str_func);

  return 0;
}

static int lf_thread_lfile_set(lua_State *L) {
  nph_thread *thread = chk_thread(L, 1);

  luaL_argcheck(L, thread != NULL, 1, "'Thread' expected");

  const char* str_file = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_file != NULL, 2, "Invalid string argument");
  thread->set_lfile(str_file);

  return 0;
}

static int lf_thread_func_get(lua_State* L) {
  nph_thread *thread = chk_thread(L, 1);
  luaL_argcheck(L, thread != NULL, 1, "'Thread' expected");

  lua_pushstring(L, thread->get_lfunc());

  return 1;
}

static int lf_thread_lfile_get(lua_State* L) {
  nph_thread *thread = chk_thread(L, 1);
  luaL_argcheck(L, thread != NULL, 1, "'Thread' expected");

  lua_pushstring(L, thread->get_lfile());

  return 1;
}

static int lf_thread_run(lua_State *L) {
  nph_thread *thread = chk_thread(L, 1);
  thread->start();
  return 0;
}

static int lf_thread_join(lua_State *L) {
  nph_thread *thread = chk_thread(L, 1);
  thread->join();
  return 0;
}

static int lf_thread_detach(lua_State *L) {
  nph_thread *thread = chk_thread(L, 1);
  thread->detach();
  return 0;
}

static int lf_thread_gc(lua_State *L) {
  Debug(5) << "gc_start";
  nph_thread **thread = reinterpret_cast<nph_thread **>(lua_touserdata(L, 1));
  luaL_argcheck(L, *thread != NULL, 1, "'Thread' expected");

  delete *thread;
  Debug(5) << "gc_end";
  return 0;
}

int luaopen_threadlib(lua_State *L) {
  static const luaL_Reg Obj_lib[] = {
    { "set_function", &lf_thread_func_set },
    { "set_file", &lf_thread_lfile_set },
    { "get_function", &lf_thread_func_get },
    { "get_file", &lf_thread_lfile_get },
    { "run", &lf_thread_run },
    { "detach", &lf_thread_detach },
    { "join", &lf_thread_join },
    { NULL, NULL }
  };

  static const luaL_Reg LuaLib_Thread[] = {
    { "new", &lf_thread_new },
    { NULL, NULL }
  };

  luaL_newlib(L, LuaLib_Thread);

  // Stack: MyLib
  luaL_newmetatable(L, NPH_THREAD);        // Stack: MyLib meta
  luaL_newlib(L, Obj_lib);
  lua_setfield(L, -2, "__index");       // Stack: MyLib meta

  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, lf_thread_gc);      // Stack: MyLib meta "__gc" fptr
  lua_settable(L, -3);                  // Stack: MyLib meta
  lua_pop(L, 1);                        // Stack: MyLib

  return 1;
}
