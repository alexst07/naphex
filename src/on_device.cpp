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
#include "on_device.h"

on_device::on_device(string dev) {
  this->init = false;
  this->alloc = false;
  this->descr = pcap_create(dev.c_str(), err);
  this->promisc = 1;
  this->to_ms = 500;
  this->device = dev;
}

on_device::on_device(const on_device& other) {
  this->init = other.init;
  this->to_ms = other.to_ms;
  this->promisc = other.promisc;

  if (other.descr != NULL) {
    this->alloc = true;
    this->descr = reinterpret_cast<pcap_t *>(malloc(sizeof(other.descr)));
    memcpy(this->descr, other.descr, sizeof(other.descr));
  }
  else
    this->descr = other.descr;

  this->device = other.device;
}

on_device&
on_device::operator=(const on_device& other) {
  this->init = other.init;
  this->to_ms = other.to_ms;
  this->promisc = other.promisc;
  this->device = other.device;

  if (other.descr != NULL) {
    this->alloc = true;
    this->descr = reinterpret_cast<pcap_t *>(malloc(sizeof(other.descr)));
    memcpy(this->descr, other.descr, sizeof(other.descr));
  }
  else
    this->descr = other.descr;

  return *this;
}

on_device::~on_device() {
  pcap_close(descr);

  if (this->alloc)
    free(descr);
}

// set the direction for which packets will be captured
bool
on_device::setdirection(int d) {
  pcap_direction_t direction;

  switch (d) {
    case 0:
      direction = PCAP_D_INOUT;
      break;

    case 1:
      direction = PCAP_D_OUT;
      break;

    case 2:
      direction = PCAP_D_IN;
      break;

    default:
      direction = PCAP_D_INOUT;
  }

  int result = pcap_setdirection(descr, direction);
  if (result == 0)
    return true;
  else
    return false;
}

// set promiscuous mode for a not-yet-activated capture handle
bool
on_device::setpromisc(int p) {
  this->promisc = p;
  if (this->descr == NULL)
    return false;

  int result = pcap_set_promisc(descr, p);
  if (result == 0)
    return true;
  else
    return false;
}

// set monitor mode for a
// not-yet-activated capture handle
bool
on_device::setrfmonitor(int m) {
  if ((pcap_can_set_rfmon(descr) == 0) && (m == 1))
    return false;

  int result = pcap_set_rfmon(descr, m);
  if (result == 0)
    return true;
  else
    return false;
}

// set the read timeout for a
// not-yet-activated capture handle
bool
on_device::settimeout(int t) {
  this->to_ms = t;
  int result = pcap_set_timeout(descr, t);
  if (result == 0)
    return true;
  else
    return false;
}

// open a device for capturing
bool
on_device::open(char *err) {
  if (this->descr != NULL) {
    int status = pcap_activate(this->descr);
    if (status != 0) {
      char str_err[PCAP_ERRBUF_SIZE];
      snprintf(str_err, PCAP_ERRBUF_SIZE, "%s", pcap_strerror(status));
      strncpy(err, str_err, strlen(str_err));
      return false;
    }
    else
      return true;
  }
  else {
    strncpy(err, this->err, strlen(this->err));
    return false;
  }
}

// Excute pcap_loop to avoid any change in the descr out of the class
int
on_device::loop(int cnt, pcap_handler callback, u_char *user) {
  mtx_loop.lock();
  int r = pcap_loop(this->descr, -1, callback, user);
  mtx_loop.unlock();
  return r;
}

// Execute pcap_breakloop()
void
on_device::breakloop() {
  mtx_break.lock();
  pcap_breakloop(this->descr);
  mtx_break.unlock();
}

// Return a pointer for pcap_t
pcap_t *
on_device::get_descr() const {
  return this->descr;
}

// Set the device to be open
void
on_device::set_device(string device) {
  this->device = device;
}

// Get the device set to be open
string
on_device::get_device() const {
  return this->device;
}
