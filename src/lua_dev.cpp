/*
 * This file is part of the Naphex project
 * Copyright (C) 2014  Alex Torres <alexst07@gmail.com>
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "nph_device.h"
#include "debug.h"

#define ONDEVICE        "ondevice"
#define OFFDEVICE       "offdevice"

static char *push_on_dev(lua_State *L, const char * name) {
  char *dev_name = reinterpret_cast<char *>(
                lua_newuserdata(L, sizeof(char)*(strlen(name)+1)));
  strncpy(dev_name, name, strlen(name)+1);
  luaL_getmetatable(L, ONDEVICE);
  lua_setmetatable(L, -2);

  return dev_name;
}

static char *chk_on_dev(lua_State *L, int index) {
  char *dev_name;
  luaL_checktype(L, index, LUA_TUSERDATA);
  dev_name = reinterpret_cast<char *>(luaL_checkudata(L, index, ONDEVICE));
  if (dev_name == NULL)
    luaL_error(L, "Argument error in Device");

  return dev_name;
}

static int lf_on_dev_new(lua_State *L) {
  const char* str_name = luaL_checkstring(L, 1);
  luaL_argcheck(L, str_name != NULL, 1, "'Invalid string name argument");

  char *dev_name = push_on_dev(L, str_name);

  if (!nph_device::exist(dev_name)) {
    nph_device::insert_on(dev_name);
  }
  else {
    luaL_error(L, "Connection '%s' already exist", dev_name);
  }

  return 1;
}

static int lf_on_dev_setdirection(lua_State *L) {
  char *name = chk_on_dev(L, 1);
  int d = luaL_checkint(L, 2);

  if (!nph_device::exist(name))
    luaL_error(L, "Device link '%s' doesn't exist", name);

  on_device *on_dev = static_cast<on_device*>(nph_device::get_conn(name));
  if (!on_dev->setdirection(d))
    luaL_error(L, "Can't set direction to '%s'", name);

  return 0;
}

static int lf_on_dev_promisc(lua_State *L) {
  char *name = chk_on_dev(L, 1);
  int p = luaL_checkint(L, 2);

  if (!nph_device::exist(name))
    luaL_error(L, "Device link '%s' doesn't exist", name);

  on_device *on_dev = static_cast<on_device*>(nph_device::get_conn(name));
  if (!on_dev->setpromisc(p))
    luaL_error(L, "Can't set to promisc mode '%s'", name);

  return 0;
}

static int lf_on_dev_rfmonitor(lua_State *L) {
  char *name = chk_on_dev(L, 1);
  int r = luaL_checkint(L, 2);

  if (!nph_device::exist(name))
    luaL_error(L, "Device link '%s' doesn't exist", name);

  on_device *on_dev = static_cast<on_device*>(nph_device::get_conn(name));
  if (!on_dev->setrfmonitor(r))
    luaL_error(L, "Can't set rf monitor '%s'", name);

  return 0;
}

static int lf_on_dev_time_out(lua_State *L) {
  char *name = chk_on_dev(L, 1);
  int t = luaL_checkint(L, 2);

  if (!nph_device::exist(name))
    luaL_error(L, "Device link '%s' doesn't exist", name);

  on_device *on_dev = static_cast<on_device*>(nph_device::get_conn(name));
  if (!on_dev->settimeout(t))
    luaL_error(L, "Can't set timeout '%s'", name);

  return 0;
}

static int lf_on_dev_open(lua_State *L) {
  char *name = chk_on_dev(L, 1);
  const char* str_dev = luaL_checkstring(L, 2);
  luaL_argcheck(L, str_dev != NULL, 2, "'Invalid string name argument");

  if (!nph_device::exist(name))
    luaL_error(L, "Device link '%s' doesn't exist", name);

  char err[PCAP_ERRBUF_SIZE];
  on_device *on_dev = static_cast<on_device*>(nph_device::get_conn(name));
  on_dev->set_device(str_dev);
  if (!on_dev->open(err))
    luaL_error(L, "Can't open device '%s': ", str_dev, err);

  return 0;
}

static int lf_on_dev_gc(lua_State *L) {
  return 0;
}

int luaopen_on_devlib(lua_State *L) {
  static const luaL_Reg Obj_lib[] = {
    { "set_direction",          &lf_on_dev_setdirection },
    { "set_promisc",            &lf_on_dev_promisc },
    { "set_rf_monitor",         &lf_on_dev_rfmonitor },
    { "set_timeout",            &lf_on_dev_time_out },
    { "open",                   &lf_on_dev_open },
    { NULL, NULL }
  };

  static const luaL_Reg LuaLib_pcap[] = {
    { "new",    &lf_on_dev_new },
    { NULL,     NULL }
  };

  luaL_newlib(L, LuaLib_pcap);

  // Stack: MyLib
  luaL_newmetatable(L, ONDEVICE);        // Stack: MyLib meta
  luaL_newlib(L, Obj_lib);
  lua_setfield(L, -2, "__index");       // Stack: MyLib meta

  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, lf_on_dev_gc);      // Stack: MyLib meta "__gc" fptr
  lua_settable(L, -3);                  // Stack: MyLib meta
  lua_pop(L, 1);                        // Stack: MyLib

  return 1;
}

// OFF Device

static int lf_off_dev_new(lua_State *L) {
  const char* str_name = luaL_checkstring(L, 1);
  luaL_argcheck(L, str_name != NULL, 1, "'Invalid string name argument");

  char *dev_name = push_on_dev(L, str_name);

  if (!nph_device::exist(dev_name)) {
    nph_device::insert_off(dev_name);
  }
  else {
    luaL_error(L, "Connection '%s' already exist", dev_name);
  }

  return 1;
}

static int lf_off_dev_offopen(lua_State *L) {
  char *name = chk_on_dev(L, 1);
  const char* str_dev = luaL_checkstring(L, 1);
  luaL_argcheck(L, str_dev != NULL, 1, "'Invalid string name argument");

  if (!nph_device::exist(name))
    luaL_error(L, "Device link '%s' doesn't exist", name);

  char err[PCAP_ERRBUF_SIZE];
  off_device *off_dev = static_cast<off_device*>(nph_device::get_conn(name));
  off_dev->set_device(str_dev);
  if (!off_dev->open(err))
    luaL_error(L, "Can't open device '%s': ", name, err);

  return 0;
}

static int lf_off_dev_gc(lua_State *L) {
  return 0;
}

int luaopen_off_devlib(lua_State *L) {
  static const luaL_Reg Obj_lib[] = {
    { "open",                   &lf_off_dev_offopen },
    { NULL, NULL }
  };

  static const luaL_Reg LuaLib_pcap[] = {
    { "new",                    &lf_off_dev_new },
    { NULL, NULL }
  };

  luaL_newlib(L, LuaLib_pcap);

  // Stack: MyLib
  luaL_newmetatable(L, OFFDEVICE);        // Stack: MyLib meta
  luaL_newlib(L, Obj_lib);
  lua_setfield(L, -2, "__index");       // Stack: MyLib meta

  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, lf_off_dev_gc);      // Stack: MyLib meta "__gc" fptr
  lua_settable(L, -3);                  // Stack: MyLib meta
  lua_pop(L, 1);                        // Stack: MyLib

  return 1;
}

/**
 * Lua function to list all devices
 */

#define IPTOSBUFFERS    12
const char *iptos(u_long in) {
  static char output[IPTOSBUFFERS][3*4+3+1];
  static short which;
  u_char *p;

  p = reinterpret_cast<u_char *>(&in);
  which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
  snprintf(output[which], sizeof(output[which]),"%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
  return output[which];
}

const char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen) {
  socklen_t sockaddrlen;

  #ifdef WIN32
  sockaddrlen = sizeof(struct sockaddr_in6);
  #else
  sockaddrlen = sizeof(struct sockaddr_storage);
  #endif


  if(getnameinfo(sockaddr,
      sockaddrlen,
      address,
      addrlen,
      NULL,
      0,
      NI_NUMERICHOST) != 0) address = NULL;

  return address;
}

static int lf_list_dev(lua_State *L) {
  pcap_if_t *alldevs;
  pcap_if_t *d;
  int i=1, y=1;
  char errbuf[PCAP_ERRBUF_SIZE];

  // Retrieve the device list from the local machine
  if (pcap_findalldevs(&alldevs, errbuf) == -1) {
    luaL_error(L, "Error in find all devs:", errbuf);
  }

  lua_newtable(L);


  // Put the device list in a Lua Table
  for (d = alldevs; d; d = d->next)
  {
    lua_newtable(L);

    pcap_addr_t *a;
    char ip6str[128];

    // name
    lua_pushstring(L, d->name);
    lua_setfield(L, -2, "name");
    // Description
    if (d->description)
      lua_pushstring(L, d->description);
    else
      lua_pushstring(L, "");

    lua_setfield(L, -2, "description");

    lua_newtable(L);
    // IP address
    for(a=d->addresses;a;a=a->next) {
      lua_newtable(L);
      switch(a->addr->sa_family) {
        case AF_INET: {
          lua_pushinteger(L, 4);
          lua_setfield(L, -2, "version");

          if (a->addr) {
            lua_pushstring(L, iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr));
            lua_setfield(L, -2, "addr");
          }

          if (a->netmask) {
            lua_pushstring(L, iptos(((struct sockaddr_in *)a->netmask)->sin_addr.s_addr));
            lua_setfield(L, -2, "netmask");
          }

          if (a->broadaddr) {
            lua_pushstring(L, iptos(((struct sockaddr_in *)a->broadaddr)->sin_addr.s_addr));
            lua_setfield(L, -2, "broadaddr");
          }

          if (a->dstaddr) {
            lua_pushstring(L, iptos(((struct sockaddr_in *)a->dstaddr)->sin_addr.s_addr));
            lua_setfield(L, -2, "dstaddr");
          }
        } break;

        case AF_INET6: {
          lua_pushinteger(L, 6);
          lua_setfield(L, -2, "version");

          if (a->addr) {
            lua_pushstring(L, ip6tos(a->addr, ip6str, sizeof(ip6str)));
            lua_setfield(L, -2, "addr");
          }
        } break;

        default: {
          lua_pushinteger(L, a->addr->sa_family);
          lua_setfield(L, -2, "version");
        }
      }
      lua_rawseti(L, -2, y++);
    }

    lua_setfield(L, -2, "ip");
    lua_rawseti(L, -2, i++);
  }


  return 1;
}

int luaopen_ldevfunc(lua_State *L) {
  lua_register(L, "list_all_devs", lf_list_dev);
  return 1;
}