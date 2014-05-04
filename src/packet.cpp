/*
 * This file is part of the Naphex project
 * Copyright (C) 2014  Alex Silva Torres <alexst07@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <assert.h>
#include <string>
#include "packet.h"
#include "debug.h"

packet::packet(std::string interface) {
  this->dev = new on_device(interface);
}

packet::packet(const packet& other) {
  this->dev = new on_device(other.dev->get_device());
}

bool
packet::send(const void* packet, size_t len) {
  const char *buf = reinterpret_cast<const char *>(packet);
#ifndef NDEBUG
  Debug(5) << "Packet size: " << len;
  for (size_t i = 0; i < len; i++)
    Debug(5) << *(buf+i);

  Debug(5) << "==========";
#endif
  int result = pcap_inject(dev->get_descr(), buf, len);
  assert(result != -1);
  Debug(5) << "SENT PACKET SIZE: " << result;
  return result;
}


packet::~packet() {
  delete this->dev;
}
