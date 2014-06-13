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
#include "monitor.h"
#include "on_device.h"
#include "off_device.h"
#include "debug.h"
#include "lua_utils.h"
#include "args.h"
#include "config_imp.h"

monitor::monitor(string interface) {
  size_t pos = interface.find("file:");
  if (pos == std::string::npos)
    this->dev = new on_device(interface);
  else {
    string file = interface.substr(pos+1);
    this->dev = new off_device(file);
  }

  this->loop_exit = false;
  this->stop = false;
  this->ran = false;

  this->L = luaL_newstate();
  this->started = false;
  this->main_func = string("");
  this->func_break = string("");
  this->func_end = string("");
  this->func_start = string("");
  this->lua_file = string("");
}

monitor::~monitor() {
  lua_close(this->L);
  delete this->dev;

  Debug(5) << "delete monitor";
}

monitor::monitor(const monitor& mon) {
  size_t pos = mon.dev->get_device().find("file:");
  if (pos == std::string::npos)
    this->dev = new on_device(mon.dev->get_device());
  else {
    string file = mon.dev->get_device().substr(pos+1);
    this->dev = new off_device(file);
  }

  this->stop = mon.stop.load(std::memory_order_relaxed);
  this->ran = mon.ran.load(std::memory_order_relaxed);
  this->loop_exit = mon.loop_exit.load(std::memory_order_relaxed);

  this->L = luaL_newstate();
  this->main_func = mon.main_func;
  this->func_end = mon.func_end;
  this->func_start = mon.func_start;
  this->lua_file = mon.lua_file;
  this->func_break = mon.func_break;
  this->started = mon.started.load(std::memory_order_relaxed);
}

void monitor::exec() {
  this->thread = std::thread(monitor::thread_func, this);
  this->thread.detach();
  this->ran = true;
}

void monitor::thread_func(monitor *mon) {
  mon->start_func();

  Debug(5) << "Pcap size: " << sizeof(mon->dev->get_descr());
  pcap_t *descr = mon->dev->get_descr();

  pcap_loop(descr, -1, monitor::process_packet, reinterpret_cast<u_char*>(mon));

  mon->end_func();
}

void monitor::process_packet(u_char *arg, const struct pcap_pkthdr* pkthdr,
                             const u_char * packet) {
  if (pkthdr->caplen == 0)
    return;

  monitor *mon = reinterpret_cast<monitor*>(arg);

  if (mon->break_loop(pkthdr, packet) == true)
    pcap_breakloop(mon->dev->get_descr());

  if (mon->stop)
    return;

  mon->action(pkthdr, packet);

  if (mon->loop_exit)
    pcap_breakloop(mon->dev->get_descr());

  return;
}

void monitor::exit() {
  this->loop_exit = true;
}

void monitor::pause() {
  this->stop = true;
}

void monitor::run() {
  if (!ran)
    this->start();
  else
    this->stop = false;
}

const pcap_t* monitor::get_dev_descr() const {
  return this->dev->get_descr();
}

void monitor::start() {
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

  // open the lua libraries used in the monitor
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

  char err[PCAP_ERRBUF_SIZE];
  if (this->dev->open(err)) {
    this->started = true;
    monitor::exec();
  }
  else {
    std::cerr << "WARNING: Fail open device: " << err << std::endl;
    return;
  }
}

void monitor::start_func() {
  if (!this->func_start.empty()) {
     np_lua_func(this->L, this->func_start.c_str(), "", "");
  }
}

void monitor::action(const struct pcap_pkthdr* pkthdr, const u_char * packet) {
  this->mtx_main_func.lock();
  if (!this->main_func.empty()) {
    laction_func(this->L, this->main_func.c_str(), packet, pkthdr->caplen);
  }
  this->mtx_main_func.unlock();
}

void monitor::end_func() {
  this->mtx_end.lock();
  if (!this->func_end.empty()) {
    np_lua_func(this->L, this->func_end.c_str(), "", "");
  }
  this->mtx_end.unlock();
}

bool monitor::break_loop(const struct pcap_pkthdr* pkthdr,
                         const u_char * packet) {
  this->mtx_break.lock();
  string str_func_break = this->func_break;
  this->mtx_break.unlock();

  if (!str_func_break.empty()) {
    int break_res;
    lfiler_func(this->L, str_func_break.c_str(),
                packet, pkthdr->caplen, &break_res);

    if (break_res == 1)
      return true;
  }

  return false;
}

// open the lua libraries used in the monitor
// at this parts should be put modules libraries
void monitor::lua_openlibs() {
  luaL_openlibs(this->L);

  luaL_requiref(L, "channel", &luaopen_channellib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "packet", &luaopen_packetlib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "args", &luaopen_argslib, 1);
  lua_pop(L, 1);

  config *conf = new config_imp(this->L);
  if (!conf->load_protocols())
    std::cerr << "WARNING: Can't load protocols libs" << std::endl;

  delete conf;
}

void monitor::set_func_start(const char *func) {
  if (this->started)
    std::cerr << "WARNING: Can't change start function" << std::endl;
  else
    this->func_start = func;
}

void monitor::set_main_func(const char* func) {
  this->mtx_main_func.lock();
  this->main_func = func;
  this->mtx_main_func.unlock();
}

void monitor::set_func_end(const char *func) {
  this->mtx_end.lock();
  this->func_end = func;
  this->mtx_end.unlock();
}

void monitor::set_func_break(const char *func) {
  this->mtx_break.lock();
  this->func_break = func;
  this->mtx_break.unlock();
}

void monitor::set_lua_file(const char *file) {
  if (this->started)
    std::cerr << "WARNING: Can't change Lua file" << std::endl;
  else
    this->lua_file = file;
}

const char* monitor::get_func_start() const {
  return this->func_start.c_str();
}

const char* monitor::get_main_func() const {
  return this->main_func.c_str();
}

const char* monitor::get_func_end() const {
  return this->func_end.c_str();
}

const char* monitor::get_func_break() const {
  return this->func_break.c_str();
}

const char* monitor::get_lua_file() const {
  return this->lua_file.c_str();
}

bool monitor::setdirection(int d) {
  return this->dev->setdirection(d);
}

bool monitor::setpromisc(int p) {
  return this->dev->setpromisc(p);
}

bool monitor::setrfmonitor(int m) {
  return this->dev->setrfmonitor(m);
}

bool monitor::settimeout(int t) {
  return this->dev->settimeout(t);
}
