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

#ifndef NAPHEX_WAIT_FILTER_H
#define NAPHEX_WAIT_FILTER_H

#include <stdio.h>
#include <pcap.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

#include "lualibs.h"
#include "device.h"

namespace naph {
  class wait_filter;
}

/**
 * \class wait_filter
 *
 * \brief Stop the execution of lua script until the the condition is satisfied
 *
 * The wait_filter class is a filter that stop the execution of lua script until
 * the condition set by the user is satisfied, a max time can be set, so even
 * the condition is not satisfied, the wait_filter return false to the user and
 * the lua script keep its execution
 */

class wait_filter {
 public:
  /**
   * Constructor
   * 
   * @param interface	Specifies the network devide interface name, ex: eth0, eth1
   */
  explicit wait_filter(string interface);

  /**
   * Copy constructor
   */
  wait_filter(const wait_filter &wf);

  /**
   * Destructor
   */
  ~wait_filter();

  /**
   * Wait execute the pcap loop, and stops the execution of lua script until filter function
   * return true
   */
  void wait();

  /**
   * Set lua function filter name
   */
  void set_func_filter(const char *func);

  /**
   * Set lua action filter name
   */
  void set_func_action(const char *func);

  /**
   * Set lua file
   */
  void set_lua_file(const char *file);

  /**
   * Set max time to wait, after wait this time,the wait
   * function return
   */
  void set_maxtime(int t);

  /**
   * Get the lua filter function name
   */
  const char* get_func_filter() const;

  /**
   * Get the lua action function name
   */
  const char* get_func_action() const;

  /**
   * Get the lua file name
   */
  const char* get_lua_file() const;

  /**
   * Get the max time to wait
   */
  unsigned int get_maxtime() const;

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
  bool set_dev_direction(int d);

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
  bool set_dev_promisc(int p);

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
  bool set_dev_rfmonitor(int m);

  /**
  *  sets the read timeout that will be used on a capture handle when the
  * handle is activated to to_ms, which is in units of milliseconds
  * 
  * @param t	Specifies the time in milliseconds
  * 
  * @return	Return true if the time was set, or false if there were some errors
  */
  bool set_dev_timeout(int t);

 private:
  /**
  * Thread function callback
  */
  static void thread_func(wait_filter *wf);

  /** 
   * Pcap loop callback function
   */
  static void process_packet(u_char *arg, const struct pcap_pkthdr* pkthdr,
                             const u_char * packet);

  /**
   * Execute lua filter function
   */
  bool filter(const struct pcap_pkthdr* pkthdr, const u_char * packet);

  /**
   * Execute lua action function
   */
  void action(const struct pcap_pkthdr* pkthdr, const u_char * packet);

  /**
   * Starts the thread to execute the pcap loop
   */
  void exec();

  /**
   * Open the lua libs that can be called by the user in the wait_filter
   * functions
   */
  void lua_openlibs();

  device *dev;
  std::thread thread;
  std::atomic<bool> started;
  std::atomic<bool> is_run;
  lua_State *L;
  std::mutex mtx_filter;
  string func_filter;
  std::mutex mtx_action;
  string func_action;
  string lua_file;
  int sec;
  time_t start_time;
};

#endif  // NAPHEX_WAIT_FILTER_H
