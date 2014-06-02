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

#include <string.h>
#include <dlfcn.h>
#include <iostream>
#include <list>
#include <string>
#include "lua_utils.h"
#include "debug.h"

void set_lua_filename(const char *name) {
  lua_filename = name;
}

const char *get_lua_filename() {
  return lua_filename;
}

// Call any kind of Lua function, where func is the name of lua function
// args is the input arguments
// res is the output of lua function
// the others arguments should be put according the args and res
void np_lua_func(lua_State *L, const char *func, const char *args,
                 const char *res, ...) {
  va_list v1;
  int nargs, nres;

  va_start(v1, res);

  /* push lua function */
  lua_getglobal(L, func);

  for (nargs = 0; *args; nargs++) {
    luaL_checkstack(L, 1, "too many arguments");

    switch (*args++) {
      case 'd':
        lua_pushnumber(L, va_arg(v1, double));
        break;

      case 'i':
        lua_pushinteger(L, va_arg(v1, int));
        break;

      case 'b':
        lua_pushboolean(L, va_arg(v1, int));
        break;

      case 's':
        lua_pushstring(L, va_arg(v1, char*));
        break;

      default:
        luaL_error(L, "invalid input option (%c)", *(args -1));
    }
  }

  /* Number of expected results */
  nres = strlen(res);
  if (lua_pcall(L, nargs, nres, 0) != 0)
    luaL_error(L, "error calling '%s': %s", func, lua_tostring(L, -1));

  nres = -nres; /* stack index of first result */

  while (*res) {
    switch (*res++) {
      case 'd': {
        int isnum;
        double n = lua_tonumberx(L, nres, &isnum);
        if (&isnum == NULL)
          luaL_error(L, "Wrong result type");
        *va_arg(v1, double *) = n;
        break;
      }

      case 'i': {
        int isint;
        int n = lua_tointegerx(L, nres, &isint);
        if (&isint == NULL)
          luaL_error(L, "Wrong result type");
        *va_arg(v1, int *) = n;
        break;
      }

      case 'b': {
        int isbool;
        isbool = lua_isboolean(L, nres);
        if (isbool == 0)
          luaL_error(L, "Wrong result type");
        int n = lua_toboolean(L, nres);
        *va_arg(v1, int *) = n;
        break;
      }

      case 's': {
        const char *s = lua_tostring(L, nres);
        if (s == NULL)
          luaL_error(L, "Wrong result type");
        *va_arg(v1, const char **) = s;
        break;
      }

      default:
        luaL_error(L, "invalid return option (%c)", *(res -1));
    }
    nres++;
  }

  va_end(v1);
}

void lfiler_func(lua_State *L, const char *func, const u_char *packet,
                 u_int len, int *ret) {
  lua_getglobal(L, func);
  if (!lua_isfunction(L, -1)) {
    luaL_error(L, "Wrong function name '%s'", func);
    lua_pop(L, 1);
    ret = NULL;
    return;
  }

  lua_pushlstring(L, reinterpret_cast<const char *>(packet), len);
  lua_pushinteger(L, len);

  if (lua_pcall(L, 2, 1, 0) != 0)
    luaL_error(L, "error calling '%s': %s", func, lua_tostring(L, -1));

  int isbool;
  isbool = lua_isboolean(L, -1);
  if (isbool == 0)
    luaL_error(L, "Wrong result type");
  int n = lua_toboolean(L, -1);
  *ret = n;
}

void laction_func(lua_State *L, const char *func, const u_char *packet,
                  u_int len) {
  if (packet == NULL)
    return;

  lua_getglobal(L, func);
  if (!lua_isfunction(L, -1)) {
    luaL_error(L, "Wrong function name '%s'", func);
    lua_pop(L, 1);
    return;
  }

  lua_pushlstring(L, reinterpret_cast<const char *>(packet), len);
  lua_pushinteger(L, len);

  if (lua_pcall(L, 2, 0, 0) != 0)
    luaL_error(L, "error calling '%s': %s", func, lua_tostring(L, -1));
}

bool llib_load(list<string> & libs, lua_State *L, const string &path) {
  void *handle;
  lua_CFunction fnc_call;
//   int (*fnc_call)(lua_State *);
  char *error;
  std::string str_lib_name;
  std::string str_fnc_name;

  luaL_getsubtable(L, LUA_REGISTRYINDEX, "_PRELOAD");
  for (list<string>::iterator it = libs.begin(); it != libs.end(); it++) {
    str_lib_name = path + "/lib" + *it + ".so";

    handle = dlopen(str_lib_name.c_str(), RTLD_LOCAL | RTLD_LAZY);
    if (!handle) {
      std::cerr << "Error: " << dlerror() << std::endl;
      return false;
    }

    dlerror();    /* Clear any existing error */

    str_fnc_name = "luaopen_" + *it;
    fnc_call = (lua_CFunction) dlsym(handle, str_fnc_name.c_str());

    if (fnc_call == NULL)  {
      std::cerr << "Error: Could not load: " << str_fnc_name << std::endl << dlerror() << std::endl;
      return false;
    }

//     luaL_requiref(L, "monitor", &luaopen_monlib, 1);
//      (*fnc_call)(L);
     Debug(5) << str_fnc_name;

     luaL_requiref(L, it->c_str(), fnc_call, 1);
     lua_pop(L, 1);

    lua_pushcfunction(L, fnc_call);
    lua_setfield(L, -2, it->c_str());
    dlclose(handle);
  }

//   for (list<string>::iterator it = libs.begin(); it != libs.end(); it++) {
//
//     lua_pushcfunction(L, fnc_call);
//     lua_setfield(L, -2, it->c_str());
//   }

  lua_pop(L, 1);

  return true;
}

bool conf_load(lua_State *L, const string &file) {
  if (luaL_loadfile(L, file.c_str()) != 0)  {
    fprintf(stderr, "Could not load: %s\n", file.c_str());
    return false;
  }

  if (lua_pcall(L, 0, 0, 0) != 0)  {
    fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    return false;
  }

  return true;
}

bool load_string(lua_State *L, const string &field, string &value) {
  lua_getglobal(L, field.c_str());

  if (!lua_isstring(L, -1)) {
    Debug(5) << "Should be a string";
    return false;
  }

  value = lua_tostring(L, -1);
  return true;
}

bool load_int(lua_State *L, const string &field, int &value) {
  lua_getglobal(L, field.c_str());

  if (!lua_isnumber(L, -1)) {
    Debug(5) << "Should be a string";
    return false;
  }

  value = lua_tointeger(L, -1);
  return true;
}

void conf_close(lua_State *L) {
  lua_close(L);
}

void get_libs(lua_State *L, const string & table, list<string> & libs) {
  libs.clear();
  Debug(5) << "Table: "<< table.c_str();

  lua_getglobal(L, table.c_str());
  lua_pushnil(L);

  while(lua_next(L, -2) != 0) {
    if(lua_isstring(L, -1))
      libs.push_back(lua_tostring(L, -1));

    lua_pop(L, 1);
  }
}