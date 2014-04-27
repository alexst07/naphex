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

#ifndef NAPHEX_THREAD_H
#define NAPHEX_THREAD_H

#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include "lualibs.h"

/**
 * \class nph_thread
 * 
 * The class nph_thread create threads by script lua,
 * the functions executed by the thread are lua functions
 * defined by the user
 */
class nph_thread {
 public:
  /**
   * Constructor
   */
  nph_thread();

  /**
   * Copy constructor
   */
  nph_thread(const nph_thread& other);

  /**
   * Destructor
   */
  ~nph_thread();

  /**
   * Starts the execution of the thread
   */
  void start();

  /**
   * suspends execution of the calling thread
   * until the target thread terminates
   */
  void join();

  /**
   * Shall indicate to the implementation that
   * storage for the thread thread can be reclaimed
   * when that thread terminates
   */
  void detach();

  /**
   * Set the lua file where are the functions
   * if a file is not defined the nph_thread will use
   * the paramter passed by command line
   */
  void set_lfile(const std::string file);

  /**
   * Set the lua function to be executed in the trhead
   */
  void set_lfunc(const std::string func);

  /**
   * Get the lua file name
   */
  const char *get_lfile() const;

  /**
   * Get the lua function name
   */
  const char *get_lfunc() const;

 private:
   /**
    * Execute the trhead
    */
  void exec();

  /**
   * Callback thread function
   */
  static void thread_func(nph_thread *thread);

  /**
   * Open the lua libs that can be called by the user in the thread
   * functions
   */
  void lua_openlibs();

  lua_State *L;
  std::thread thread;
  std::string lua_file;
  std::string lfunc;
  std::atomic<bool> is_running;
};

#endif  // NAPHEX_THREAD_H
