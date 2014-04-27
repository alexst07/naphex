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

#include "nph_device.h"

nph_device* nph_device::_instance = NULL;
std::mutex nph_device::mtx_dev;
std::map<string, device*> nph_device::device_conns;

nph_device::~nph_device() {

}

// Method that creates a single
// instance of the object nph_device
nph_device* nph_device::Instance() {
  if (_instance == NULL) {
    _instance = new nph_device;
  }

  return _instance;
}

bool nph_device::exist(string conn) {
  std::map<string, device*>::iterator it;
  mtx_dev.lock();
  it = device_conns.find(conn);
  mtx_dev.unlock();
  if (it == device_conns.end())
    return false;
  else
    return true;
}

bool nph_device::insert_on(string conn) {
  on_device *dev_con;

  if (!exist(conn)) {
    std::map<string,device*>::iterator it;
    dev_con = new on_device("");
    mtx_dev.lock();
    it = device_conns.begin();
    device_conns.insert(it, std::pair<string,device*>(conn, dev_con));
    mtx_dev.unlock();
    return true;
  }
  else
    return false;
}

bool nph_device::insert_off(string conn) {
  off_device *dev_con;

  if (!exist(conn)) {
    std::map<string,device*>::iterator it;
    dev_con = new off_device("");
    mtx_dev.lock();
    it = device_conns.begin();
    device_conns.insert(it, std::pair<string,device*>(conn, dev_con));
    mtx_dev.unlock();
    return true;
  }
  else
    return false;
}

bool nph_device::erase(string conn) {
    if (exist(conn)) {
    std::map<string,device*>::iterator it;
    mtx_dev.lock();
    it = device_conns.find(conn);
    device_conns.erase(it);
    mtx_dev.unlock();
    return true;
  }
  return false;
}

device *nph_device::get_conn(string conn) {
    if (exist(conn)) {
    std::map<string,device*>::iterator it;
    mtx_dev.lock();
    it = device_conns.find(conn);
    mtx_dev.unlock();
    return it->second;
  }
  return NULL;
}