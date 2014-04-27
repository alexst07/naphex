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

#ifndef NPH_DEVICE_H
#define NPH_DEVICE_H

#include <map>
#include <string>
#include <mutex>

#include "device.h"
#include "off_device.h"
#include "on_device.h"

using std::string;

class nph_device {
 public:
  static nph_device* Instance();
  ~nph_device();

  static bool insert_on(string conn);
  static bool insert_off(string conn);
  static bool erase(string conn);
  static bool exist(string conn);
  static device *get_conn(string conn);

 private:
   static std::mutex mtx_dev;
   static std::map<string, device*> device_conns;
   static nph_device* _instance;
};

#endif // NPH_DEVICE_H
