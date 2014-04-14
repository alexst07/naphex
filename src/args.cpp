/*
 * <one line to give the library's name and an idea of what it does.>
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

#include "args.h"

#define ARGS        "args"

args* args::_instance = NULL;
std::vector<std::string> args::items;

args::~args() {

}

args* args::instance() {
  if (_instance == NULL) {
    _instance = new args;
  }

  return _instance;
}

std::string args::get(const size_t index) {
  return items[index];
}

void args::insert(const std::string item) {
  items.push_back(item);
}

size_t args::nargs() {
  return reinterpret_cast<size_t>(items.size());
}

static int lf_args_get(lua_State *L) {
  size_t index = luaL_checkunsigned(L, 1);

  if (index < args::nargs())
    lua_pushstring(L, args::get(index).c_str());
  
  return 1;
}

static int lf_args_size(lua_State *L) {
  lua_pushunsigned(L, args::nargs());
  return 1;
}

int luaopen_argslib(lua_State *L) {
  static const luaL_Reg Obj_lib[] = {
    { "get",	&lf_args_get },
    { "size",	&lf_args_size },
    { NULL, NULL }
  };

  luaL_newlib(L, Obj_lib);
  return 1;
}