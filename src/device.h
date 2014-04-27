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

#ifndef NAPHEX_DEVICE_H
#define NAPHEX_DEVICE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pcap/pcap.h>
#include <string>

using std::string;

namespace naph {
  class device;
}

/**
 * \class device
 *
 * \brief A interface to device
 *
 * The abstract device class is a interface to device
 * in the class is declared basic functions tha is used
 * in online and offline devices
 */

class device {
 public:
  /**
   * Gets the pcap descript of a device
   */
  virtual pcap_t * get_descr() const = 0;

  /**
   * Specifies the network device, ex: eth0, eth1
   * 
   * @param device	Network device name
   */
  virtual void set_device(string device) = 0;

  /**
   * Gets the network device
   */
  virtual string get_device() const = 0;

  /**
   * Open a network device for packet capture
   * 
   * @param err Is a pointer to the error message
   * 
   * @return	Returns true if the device was opened 
   *		successfully, or false if there were errors
   */
  virtual bool open(char *err) = 0;

  /**
   * \brief		Starts the loop of libpcap
   * 
   * processes packets from a live capture or ``savefile'' until cnt packets
   * are processed, the end of the ``savefile'' is reached when reading from a
   * ``savefile'', pcap_breakloop() is called, or an error occurs. It does
   * not return when live read timeouts occur. A value of -1 or 0 for cnt is
   * equivalent to infinity, so that packets are processed until another
   * ending condition occurs. 
   * 
   * @param cnt		Number of packet to be received
   * @param callback	Callback function pointer
   * @param user	Data to be passed to callback function
   * 
   * @return		returns 0 if cnt is exhausted or if, when reading
   * 			from a ``savefile'', no more packets are available.
   * 			It returns -1 if an error occurs or -2 if the loop
   * 			terminated due to a call to pcap_breakloop() before
   * 			any packets were processed. It does not return when
   * 			live read timeouts occur; instead, it attempts to 
   * 			read more packets. 
   */
  virtual int loop(int cnt, pcap_handler callback, u_char *user) = 0;

  /**
   * \brief		Break the receive packet loop
   * 
   * sets a flag that will force pcap_dispatch() or pcap_loop() to return 
   * rather than looping; they will return the number of packets that have 
   * been processed so far, or -2 if no packets have been processed so far. 
   */
  virtual void breakloop() = 0;

  /**
   * is used to specify a direction that packets will be captured.
   * d is one of the constants PCAP_D_IN, PCAP_D_OUT or PCAP_D_INOUT.
   * PCAP_D_IN will only capture packets received by the device,
   * PCAP_D_OUT will only capture packets sent by the device and 
   * PCAP_D_INOUT will capture packets received by or sent by the device.
   * PCAP_D_INOUT is the default setting if this function is not called. 
   * 
   * @param d	Specifies the direction
   * @return	Return true if the direction was set, or true if there were
   * 		some errors
   */
  virtual bool setdirection(int d) {}

  /**
   * sets whether promiscuous mode should be set on a capture handle when
   * the handle is activated. If promisc is non-zero, promiscuous mode willbe set,
   * otherwise it will not be set
   * 
   * @param p	Set the promisc mode, if p is different fo zero,
   * 		a promisc mode is set
   * 
   * @return	Return true if the promisc mode was set, and false if there were
   * 		some errors
   */
  virtual bool setpromisc(int p) {}

  /**
   * sets whether monitor mode should be set on a capture handle when the handle
   * is activated. If rfmon is non-zero, monitor mode will be set, otherwise
   * it will not be set
   * 
   * @param m	Set rf monitor mode if m is non-zero
   * 
   * @return	Return true if the promisc mode was set, and false if there were
   * 		some errors
   */
  virtual bool setrfmonitor(int m) {}

  /**
   *  sets the read timeout that will be used on a capture handle when the
   * handle is activated to to_ms, which is in units of milliseconds
   * 
   * @param t	Specifies the time in milliseconds
   * 
   * @return	Return true if the time was set, or false if there were some errors
   */
  virtual bool settimeout(int t) {}

  /**
   * Destructor
   */
  virtual ~device(){}
};

#endif  // NAPHEX_DEVICE_H
