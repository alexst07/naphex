/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
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

#include "config_imp.h"
#include "lua_utils.h"
#include "define.h"

#define LUA_LIBS_TABLE  "extensions"
#define LUA_LIBS_PATH   "extension_dir"

using std::string;

config_imp::config_imp() {
  conf_load(this->L, NAPHEX_CONFIG_FILE);
}

bool config_imp::load_protocols() {
  list<string> libs;
  string path;

  get_libs(this->L, LUA_LIBS_TABLE, libs);
  if (!load_string(this->L, LUA_LIBS_PATH, path))
    return false;

  if (!llib_load(libs, this->L, path))
    return false;

  return true;
}

config_imp::~config_imp() {
  conf_close(this->L);
}