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

#include <string>
#include "nph_thread.h"
#include "debug.h"
#include "lua_utils.h"
#include "args.h"
#include "config_imp.h"

nph_thread::nph_thread() {
  this->is_running = false;
  this->L = luaL_newstate();
  this->lua_file = std::string("");
  this->lfunc = std::string("");
}

nph_thread::nph_thread(const nph_thread& other) {
  this->is_running = other.is_running.load(std::memory_order_relaxed);
  this->lua_file = other.lua_file;
  this->lfunc = other.lfunc;
  this->L = luaL_newstate();
}

nph_thread::~nph_thread() {
  lua_close(this->L);

  if (this->is_running)
    this->thread.detach();
}

void
nph_thread::exec() {
  this->thread = std::thread(nph_thread::thread_func, this);
}

void
nph_thread::thread_func(nph_thread *thread) {
  // execute the lua function
  np_lua_func(thread->L, thread->get_lfunc(), "", "");
}

void
nph_thread::start() {
  if (this->lua_file.empty()) {
    if (get_lua_filename() != NULL) {
      const char *luaf_tmp = get_lua_filename();
      this->lua_file = luaf_tmp;
    }
    else {
      luaL_error(this->L, "ERROR: Lua file not defined");
      return;
    }
  }

  if (this->lfunc.empty()) {
    std::cerr << "WARING: Function is not defined" << std::endl;
    return;
  }

  // open the lua libraries used in the thread
  // at this parts should be put modules libraries
  lua_openlibs();

  if (luaL_loadfile(this->L, this->lua_file.c_str()) != 0) {
    luaL_error(this->L, "ERROR: Could not load: %s", this->lua_file.c_str());
    return;
  }

  if (lua_pcall(this->L, 0, 0, 0) != 0)   {
    std::cerr << "ERROR: " << lua_tostring(this->L, -1) << std::endl;
    lua_pop(this->L, 1);
    return;
  }

  this->is_running = true;
  this->exec();
}

void
nph_thread::join() {
  if (this->is_running)
    this->thread.join();
  this->is_running = false;
}

void
nph_thread::detach() {
  if (this->is_running)
    this->thread.detach();
  this->is_running = false;
}

void
nph_thread::lua_openlibs() {
  luaL_openlibs(this->L);

  luaL_requiref(L, "channel", &luaopen_channellib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "packet", &luaopen_packetlib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "args", &luaopen_argslib, 1);
  lua_pop(L, 1);

  config *conf = new config_imp(L);
  if (!conf->load_protocols())
    std::cerr << "WARNING: Can't load protocols libs" << std::endl;

  delete conf;
}

void
nph_thread::set_lfile(const std::string file) {
  this->lua_file = file;
}

void
nph_thread::set_lfunc(const std::string func) {
  this->lfunc = func;
}

const char *
nph_thread::get_lfile() const {
  return this->lua_file.c_str();
}

const char *
nph_thread::get_lfunc() const {
  return this->lfunc.c_str();
}

