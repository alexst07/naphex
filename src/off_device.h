/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#ifndef OFF_DEVICE_H
#define OFF_DEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include "device.h"

namespace naph {
  class off_device;
}

class off_device : public device {
 public:
  off_device(string file);
  off_device(const off_device& other);
  off_device& operator=(const off_device& other);
  ~off_device();

  bool open(char *err) override;
  pcap_t * get_descr() const override;
  void set_device(string device) override;
  string get_device() const override;
  int loop(int cnt, pcap_handler callback, u_char *user) override;
  void breakloop() override;

 private:
  u_int precision;
  bool init;
  pcap_t * descr;
  string file;
  bool alloc;
};

#endif // OFF_DEVICE_H
