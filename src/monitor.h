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

#ifndef NAPHEX_MONITOR_H
#define NAPHEX_MONITOR_H

#include <stdio.h>
#include <stdlib.h>
#include <pcap/pcap.h>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>

#include "device.h"
#include "lualibs.h"

namespace naph {
  class monitor;
}

/**
 * \class monitor
 *
 * \brief Class to monitor a device
 *
 * The monitor class is responsible to monitor a specified device.
 * The monitor create a pcap loop, and the lua function defined by
 * user is called inside the callback function
 */

class monitor {
 public:
  /**
   * Constructor
   * 
   * @param interface	Specifies the network device interface
   * 			to be monitored, ex: eth0, eth1
   */
  explicit monitor(string interface);

  /**
   * Copy constructor
   */
  monitor(const monitor& other);

  /**
   * Destructor
   */
  ~monitor();

  /**
   * Gets the pcap descript of a device
   */
  const pcap_t * get_dev_descr() const;

  /**
   * Exit of the pcap loop and terminate the monitor
   */
  void exit();

  /**
   * Pause the monitor
   */
  void pause();

  /**
   * Run the monitor, if the monitor was started and it is paused
   * so only run the monitor, if the loop was not created, call start
   */
  void run();

  /**
   * Start the pcap loop, and set the necessary variables
   */
  void start();

  /**
   * \brief Set the start function
   * 
   * Start function is executed only one time before to start
   * the pcap loop
   */
  void set_func_start(const char *func);

  /**
   * \brief Set the filter function
   * 
   * Filter function is executed inside the loop to filter packets
   * according the lua function defined by the user
   */
  void set_func_filter(const char *func);

  /**
   * \brief Set the action function
   * 
   * Action function is executed inside the pcap loop, and every time
   * that the filter function return true, the action function is
   * executed
   */
  void set_func_action(const char *func);

  /**
   * \brief Set the end function
   * 
   * The end function is executed when exit is called
   */
  void set_func_end(const char *func);

  /**
   * \brief Set break function
   * 
   * Break function is executued inside the pcap loop
   * and when it returns true the pcap loop is broken
   */
  void set_func_break(const char *func);

  /**
   * \brief Specifies the lua file
   * 
   * The Lua file contains the Lua functions used in the monitor,
   * it can be spent on implementation of naphex by command line,
   * or can be passed in the call monitor
   */
  void set_lua_file(const char *file);

  /**
   * Gets the start function
   */
  const char* get_func_start() const;

  /**
   * Gets the filter function
   */
  const char* get_func_filter() const;

  /**
   * Gets the action function
   */
  const char* get_func_action() const;

  /**
   * Gets the end function
   */
  const char* get_func_end() const;

  /**
   * Gets the break function
   */
  const char* get_func_break() const;

  /**
   * Gets the lua file
   */
  const char* get_lua_file() const;

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
  bool setdirection(int d);

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
  bool setpromisc(int p);

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
  bool setrfmonitor(int m);

  /**
  *  sets the read timeout that will be used on a capture handle when the
  * handle is activated to to_ms, which is in units of milliseconds
  * 
  * @param t	Specifies the time in milliseconds
  * 
  * @return	Return true if the time was set, or false if there were some errors
  */
  bool settimeout(int t);

 private:
  /**
   * Execute the lua start function
   */
  void start_func();

  /** 
   * Execute the lua filter function
   */
  bool filter(const struct pcap_pkthdr* pkthdr, const u_char * packet);

  /**
   * Execute the lua action function
   */
  void action(const struct pcap_pkthdr* pkthdr, const u_char * packet);

  /**
   * Execute the lua break loop function
   */
  bool break_loop(const struct pcap_pkthdr* pkthdr, const u_char * packet);

  /** 
   * Execute the lua end function
   */
  void end_func();

  /**
   * Starts the thread responsible to execute the pcap loop
   */
  void exec();

  /**
   * Callback function responsible to process the packet in the pcap loop
   */
  static void process_packet(u_char *arg, const struct pcap_pkthdr* pkthdr,
                      const u_char * packet);

  /**
   * Callback function to be passed to thread
   */
  static void thread_func(monitor *mon);

  /**
   * Open lua libs that can be used by the user in the lua monitor functions
   */
  void lua_openlibs();

  device *dev;
  std::atomic<bool> loop_exit;
  std::atomic<bool> stop;
  std::atomic<bool> ran;
  std::thread thread;

  string dev_interface;
  string lua_file;
  std::mutex mtx_filter;
  string func_filter;
  std::mutex mtx_action;
  string func_action;
  string func_start;
  std::mutex mtx_end;
  string func_end;
  std::mutex mtx_break;
  string func_break;
  lua_State *L;
  std::atomic<bool> started;
};

#endif  // NAPHEX_MONITOR_H
