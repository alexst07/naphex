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

#include "lua_utils.h"

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
