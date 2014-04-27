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

#ifndef NAPHEX_ARGS_H_
#define NAPHEX_ARGS_H_

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <stddef.h>
#include <string>
#include <vector>

/**
 * \class args
 * 
 * \brief Provide the arguments command line
 * 
 * Provide a single access point to the arguments
 * command line
 */

class args {
 public:
  /**
   * Creates a single instance of the object
   */
  static args* instance();

  /**
   * Destructor
   */
  ~args();

  /**
   * takes the argument passed by the command line 
   * given its position in the array of string
   * 
   * @param[size_t]	index Position of the element you want to read
   * @return		Command line argument
   */
  static std::string get(const size_t index);

  /**
   * Insert a new element in the vector or command line
   * arguments
   * 
   * @param[std::string]	item Argument to store
   */
  static void insert(const std::string item);

  /**
   * Gets the numbers of arguments command line
   */
  static size_t nargs();

 private:
  static std::vector<std::string> items;
  static args* _instance;
};

/**
 * Register a new lua lib
 */
int luaopen_argslib(lua_State *L);
#endif  // NAPHEX_ARGS_H_
