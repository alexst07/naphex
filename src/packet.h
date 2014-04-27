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

#ifndef NAHPEX_PACKET_H
#define NAHPEX_PACKET_H

#include <pcap/pcap.h>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include "device.h"
#include "on_device.h"

/**
 * \class packet
 *
 * \brief Create a packet to send
 *
 * The packet class create a packet and send it to the
 * specified network device interface
 */

class packet {
 public:
  /**
   * Constructor
   * 
   * @param interface	Specifies the network device interface name
   */
  explicit packet(std::string interface);

  /**
   * Copy constructor
   */
  packet(const packet& other);

  /**
   * Send a packet
   * 
   * @param packet	Pointer to data
   * @param len		Length of packet
   */
  bool send(const void* packet, size_t len);

  /**
   * Destructor
   */
  ~packet();

 private:
  device *dev;
};

#endif  // NAHPEX_PACKET_H
