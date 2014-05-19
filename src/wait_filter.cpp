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

#include "wait_filter.h"
#include "lua_utils.h"
#include "on_device.h"
#include "off_device.h"
#include "debug.h"
#include "args.h"
#include "config_imp.h"

wait_filter::wait_filter(string interface) {
  size_t pos = interface.find("file:");
  if (pos == std::string::npos)
    this->dev = new on_device(interface);
  else {
    string file = interface.substr(pos+1);
    this->dev = new off_device(file);
  }

  this->L = luaL_newstate();
  this->started = false;
  this->start_time = -1;
  this->is_run = false;
  this->sec = -1;
  this->lua_file = std::string("");
  this->func_action = std::string("");
  this->func_filter = std::string("");
}

wait_filter::~wait_filter() {
  lua_close(this->L);

  if (is_run)
    thread.detach();

  delete this->dev;
}

wait_filter::wait_filter(const wait_filter &wf) {
  this->L = luaL_newstate();
  memcpy(this->L, wf.L, sizeof(wf.L));

  size_t pos = wf.dev->get_device().find("file:");
  if (pos == std::string::npos)
    this->dev = new on_device(wf.dev->get_device());
  else {
    string file = wf.dev->get_device().substr(pos+1);
    this->dev = new off_device(file);
  }

  this->started = wf.started.load(std::memory_order_relaxed);
  this->func_action = wf.func_action;
  this->func_filter = wf.func_filter;
  this->lua_file = wf.lua_file;
  this->sec = wf.sec;
  this->start_time = wf.start_time;
  this->is_run = wf.is_run.load(std::memory_order_relaxed);
}

void
wait_filter::exec() {
  time(&this->start_time);
  this->is_run = true;
  this->thread = std::thread(wait_filter::thread_func, this);

  this->started = true;

  this->thread.join();
  this->is_run = false;
  Debug(5) << "Thread finished";
}

void
wait_filter::wait() {
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

  if (this->func_filter.empty()) {
    std::cerr << "WARNING: Filter function is not defined" << std::endl;
    return;
  }

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
    wait_filter::exec();
  }
  else {
    std::cerr << "WARNING: Fail open device: " << err << std::endl;
    return;
  }
}

void
wait_filter::thread_func(wait_filter *wf) {
  pcap_t *descr = wf->dev->get_descr();
  pcap_loop(descr, -1, wait_filter::process_packet,
            reinterpret_cast<u_char*>(wf));
}

void
wait_filter::process_packet(u_char *arg, const struct pcap_pkthdr* pkthdr,
                             const u_char * packet) {
  wait_filter *wf = reinterpret_cast<wait_filter*>(arg);

  if (wf->filter(pkthdr, packet) == true) {
    wf->action(pkthdr, packet);
    pcap_breakloop(wf->dev->get_descr());
  }

  if (wf->sec > 0) {
    time_t current_time;
    time(&current_time);
    double seconds = difftime(current_time, wf->start_time);
    Debug(5) << "seconds diff: " << seconds << "senconds given: " << wf->sec;
    if (lrint(floor(seconds)) > lrint(wf->sec))
      pcap_breakloop(wf->dev->get_descr());
  }

  return;
}

bool
wait_filter::filter(const struct pcap_pkthdr* pkthdr,
                         const u_char * packet) {
  this->mtx_filter.lock();
  string str_func_filter = this->func_filter;
  this->mtx_filter.unlock();

  if (!str_func_filter.empty()) {
    int filter_res;
    lfiler_func(this->L, str_func_filter.c_str(),
                packet, pkthdr->caplen, &filter_res);

    if (filter_res == 1)
      return true;
  }

  return false;
}

void
wait_filter::action(const struct pcap_pkthdr* pkthdr,
                         const u_char * packet) {
  this->mtx_action.lock();
  if (!this->func_action.empty()) {
    laction_func(this->L, this->func_action.c_str(), packet, pkthdr->caplen);
  }
  this->mtx_action.unlock();
}

void
wait_filter::set_func_filter(const char *func) {
  this->mtx_filter.lock();
  this->func_filter = func;
  this->mtx_filter.unlock();
}

void
wait_filter::set_func_action(const char *func) {
  this->mtx_action.lock();
  this->func_action = func;
  this->mtx_action.unlock();
}

void
wait_filter::set_maxtime(int t) {
  if (this->started)
    std::cerr << "WARNING: Can't change MaxWaitTime" << std::endl;
  else
    this->sec = t;

  Debug(5) << "maxtime: " << this->sec;
}

void
wait_filter::set_lua_file(const char *file) {
  if (this->started)
    std::cerr << "WARNING: Can't change Lua file" << std::endl;
  else
    this->lua_file = file;
}

const char*
wait_filter::get_func_filter() const {
  return this->func_filter.c_str();
}

const char*
wait_filter::get_func_action() const {
  return this->func_action.c_str();
}

const char*
wait_filter::get_lua_file() const {
  return this->lua_file.c_str();
}

unsigned int wait_filter::get_maxtime() const {
  return this->sec;
}

void
wait_filter::lua_openlibs() {
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

bool
wait_filter::set_dev_direction(int d) {
  return this->dev->setdirection(d);
}

bool
wait_filter::set_dev_promisc(int p) {
  return this->dev->setpromisc(p);
}

bool
wait_filter::set_dev_rfmonitor(int m) {
  return this->dev->setrfmonitor(m);
}

bool
wait_filter::set_dev_timeout(int t) {
  return this->dev->settimeout(t);
}
