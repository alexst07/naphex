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

#ifndef NAPHEX_CONFIG_H
#define NAPHEX_CONFIG_H

#include <string>
#include <list>

using std::list;
using std::string;

namespace naph {
  class config;
}

/**
 * \class config
 *
 * \brief A interface to config files
 *
 * The abstract config class is a interface to config files
 */

class config {
 public:
  /**
   * Load the protocols libs
   */
  virtual bool load_protocols() = 0;

  /**
   * Destructor
   */
  virtual ~config(){}
};

#endif  // NAPHEX_CONFIG_H