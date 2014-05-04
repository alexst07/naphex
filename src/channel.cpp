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
#include <map>
#include <mutex>
#include <thread>
#include <utility>

#include "channel.h"

channel* channel::_instance = NULL;
std::mutex channel::mtx;
std::map<std::string, std::string> channel::channels;

channel::~channel() {
}

//
// Method that creates a single instance of the object channel
//
channel*
channel::Instance() {
  if (_instance == NULL) {
    _instance = new channel;
  }

  return _instance;
}

bool
channel::exist(std::string ch_name) {
  std::map<std::string, std::string>::iterator it;
  mtx.lock();
  it = channels.find(ch_name);
  mtx.unlock();
  if (it == channels.end())
    return false;
  else
    return true;
}

bool
channel::insert(std::string ch_name, std::string value) {
  if (!exist(ch_name)) {
    std::map<std::string, std::string>::iterator it;
    mtx.lock();
    it = channels.begin();
    channels.insert(it, std::pair<std::string, std::string>(ch_name, value));
    mtx.unlock();
    return true;
  }
  else
    return false;
}

bool
channel::erase(std::string ch_name) {
    if (exist(ch_name)) {
    std::map<std::string, std::string>::iterator it;
    mtx.lock();
    it = channels.find(ch_name);
    channels.erase(it);
    mtx.unlock();
    return true;
  }
  return false;
}

std::string
channel::get(std::string ch_name) {
    if (exist(ch_name)) {
    std::map<std::string, std::string>::iterator it;
    mtx.lock();
    it = channels.find(ch_name);
    mtx.unlock();
    return it->second;
  }
  return NULL;
}

bool channel::set(std::string ch_name, std::string value) {
  if (!exist(ch_name)) {
    return false;
  }
  else {
    std::map<std::string, std::string>::iterator it;
    mtx.lock();
    it = channels.find(ch_name);
    channels[it->first] = value;
    mtx.unlock();
    return true;
  }
}
