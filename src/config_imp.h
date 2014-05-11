/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
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

#ifndef NAPHEX_CONFIG_IMP_H
#define NAPHEX_CONFIG_IMP_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "nph_config.h"

class config_imp : public config {
 public:
  /**
   * Constructor
   */
  config_imp();

  /**
   * override load_protocols method
   */
  bool load_protocols() override;

  /**
   * Destructor
   */
  ~config_imp();

 private:
  lua_State *L;
  bool loaded;
};

#endif  // NAPHEX_CONFIG_IMP_H
