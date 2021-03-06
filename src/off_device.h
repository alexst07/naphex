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

#ifndef NAPHEX_OFF_DEVICE_H
#define NAPHEX_OFF_DEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "device.h"

namespace naph {
  class off_device;
}

/**
 * \class off_device
 * 
 * The off_device is a class to open a saved file with the packets
 * so the packets are received offline.
 * This class is derivated of abastract class device
 */
class off_device : public device {
 public:
  /**
   * Constructor
   * 
   * @param file	Specifies the pcap file to be open
   */
  explicit off_device(string file);

  /**
   * Copy constructor
   */
  off_device(const off_device& other);

  /**
   * Overload oprator =
   */
  off_device& operator=(const off_device& other);

  /**
   * Destructor
   */
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

#endif  // NAPHEX_OFF_DEVICE_H
