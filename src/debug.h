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

#ifndef NAPHEX_DEBUG_H
#define NAPHEX_DEBUG_H

#ifndef NDEBUG
static int nph_debug_level = 5;
#endif

class Debug {
 public:
  explicit Debug(int level)
#ifndef NDEBUG
  :m_output(level <= nph_debug_level)
#endif
  {}

  template<typename T>
  Debug& operator<<(T const& t) {
#ifndef NDEBUG
    if (m_output) {
      std::cerr << t << std::endl;
      return *this;
    }
    else
#endif
      return *this;
    }
 private:
#ifndef NDEBUG
  bool m_output;
#endif
};

#endif  // NAPHEX_DEBUG_H
