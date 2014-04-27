/*
 * This file is part of the Naphex project
 * Copyright (C) 2014  Alex Silva Torres <alexst07@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "packet.h"
#include "debug.h"

#define PACKET  "packet"

static packet* push_packet(lua_State *L, packet wf) {
  // create a userdatum to store wait_filter address
  packet **pk = reinterpret_cast<packet **>(
                lua_newuserdata(L, sizeof(packet)));
  *pk = new packet(wf);

  luaL_getmetatable(L, PACKET);
  lua_setmetatable(L, -2);

  return *pk;
}

static packet* chk_packet(lua_State *L, int index) {
  packet *pk;
  luaL_checktype(L, index, LUA_TUSERDATA);
  pk = *reinterpret_cast<packet **>(luaL_checkudata(L, index, PACKET));
  if (pk == NULL)
    luaL_error(L, "Argument error in Packet");

  return pk;
}

static int lf_packet_new(lua_State *L) {
  int argc = lua_gettop(L);
  Debug(5) << "Number of param: " << argc;

  if (argc != 1)
    luaL_error(L, "Packet expects 1 argument");

  const char* str_device = luaL_checkstring(L, 1);

  push_packet(L, packet(str_device));

  luaL_getmetatable(L, PACKET);
  lua_setmetatable(L, -2);
  Debug(5) << "Object created!";
  return 1;
}

static int lf_packet_send(lua_State *L) {
  packet *pk = chk_packet(L, 1);

  luaL_argcheck(L, pk != NULL, 1, "'Packet' expected");

  const char* buf = luaL_checkstring(L, 2);
  int len = luaL_checkinteger(L, 3);
  luaL_argcheck(L, buf != NULL, 2, "Invalid packet buffer argument");
  pk->send(buf, len);

  return 0;
}

static int lf_packet_gc(lua_State *L) {
  Debug(5) << "gc_start";
  packet **pk = reinterpret_cast<packet **>(lua_touserdata(L, 1));
  luaL_argcheck(L, *pk != NULL, 1, "'Packet' expected");

  delete *pk;
  Debug(5) << "gc_end";
  return 0;
}

int luaopen_packetlib(lua_State *L) {
  static const luaL_Reg Obj_lib[] = {
    { "send", &lf_packet_send },
    { NULL, NULL }
  };

  static const luaL_Reg LuaLib_PK[] = {
    { "new", &lf_packet_new },
    { NULL, NULL }
  };

  luaL_newlib(L, LuaLib_PK);

  // Stack: MyLib
  luaL_newmetatable(L, PACKET);        // Stack: MyLib meta
  luaL_newlib(L, Obj_lib);
  lua_setfield(L, -2, "__index");       // Stack: MyLib meta

  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, lf_packet_gc);      // Stack: MyLib meta "__gc" fptr
  lua_settable(L, -3);                  // Stack: MyLib meta
  lua_pop(L, 1);                        // Stack: MyLib

  return 1;
}
