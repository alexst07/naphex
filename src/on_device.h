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

#ifndef NAPHEX_ON_DEVICE_H
#define NAPHEX_ON_DEVICE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <mutex>

#include "device.h"

namespace std {
template<class _Tp >
class atomic;
}

namespace naph {
  class on_device;
}

/**
 * \class on_device
 * 
 * The clss on_device is derivate of abstract class device
 * and define the virtuals functions of device class.
 * on_device is used to online network device interface,
 * it means that the packets are received in real time by this
 * class
 */
class on_device : public device {
 public:
  /**
   * Constructor
   * 
   * @param device	Specifies the network device name
   */
  explicit on_device(string device);

  /**
   * Copy constructor
   */
  on_device(const on_device& other);

  /**
   * Overload operator =
   */
  on_device& operator=(const on_device& other);

  /**
   * Destructor
   */
  ~on_device();

  bool open(char *err) override;
  pcap_t * get_descr() const override;
  void set_device(string device) override;
  string get_device() const override;
  int loop(int cnt, pcap_handler callback, u_char *user) override;
  void breakloop() override;

  bool setdirection(int d) override;
  bool setpromisc(int p) override;
  bool setrfmonitor(int m) override;
  bool settimeout(int t) override;

 private:
  int promisc;
  int to_ms;
  bool init;
  pcap_t * descr;
  string device;
  bool alloc;
  std::mutex mtx_loop;
  std::mutex mtx_break;
  char err[PCAP_ERRBUF_SIZE];
};

#endif  // NAPHEX_ON_DEVICE_H
