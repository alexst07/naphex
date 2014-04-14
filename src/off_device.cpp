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

#include "off_device.h"

off_device::off_device(string file) {
  this->precision = 0;
  this->init = false;
  this->descr = NULL;
  this->file = file;
  this->alloc = false;
}

off_device::off_device(const off_device& other) {
  this->init = other.init;
  this->precision = other.precision;
  this->file = file;

  if (other.descr != NULL) {
    this->alloc = true;
    this->descr = reinterpret_cast<pcap_t *>(malloc(sizeof(other.descr)));
    memcpy(this->descr, other.descr, sizeof(other.descr));
  }
  else
    this->descr = other.descr;
}

off_device& off_device::operator=(const off_device& other) {
  this->init = other.init;
  this->precision = other.precision;
  this->file = file;

  if (other.descr != NULL) {
    this->alloc = true;
    this->descr = reinterpret_cast<pcap_t *>(malloc(sizeof(other.descr)));
    memcpy(this->descr, other.descr, sizeof(other.descr));
  }
  else
    this->descr = other.descr;

  return *this;
}

off_device::~off_device() {
  if (init)
    pcap_close(descr);
}

// open a file for capturing
bool off_device::open(char *err) {
  if (pcap_open_offline(file.c_str(), err) == NULL)
    return false;
  else {
    init = true;
    return true;
  }
}

// Excute pcap_loop to avoid any change in the descr out of the class
int off_device::loop(int cnt, pcap_handler callback, u_char *user) {
  int r = pcap_loop(this->descr, -1, callback, user);
  return r;
}

// Execute pcap_breakloop()
void off_device::breakloop() {
  pcap_breakloop(this->descr);
}

// Return a pointer for pcap_t
pcap_t * off_device::get_descr() const {
  return this->descr;
}

// Set the device to be open
void off_device::set_device(string file) {
  this->file = file;
}

// Get the device set to be open
string off_device::get_device() const {
  return this->file;
}