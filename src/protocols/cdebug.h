/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2013  Alex Torres <email>
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

#ifndef INCLUDE_NPH_DEBUG_H_
#define INCLUDE_NPH_DEBUG_H_

#include "define.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#ifdef NAPHEX_NDEBUG
# define NDEBUG
#endif

// This peace of code was based in
// .H.M. Dassen, I.G. Sprinkhuizen-Kuyper
// Debugging C and C++ code in a Unix environment

static int msglevel = 5; /* the higher, the more messages... */

#if defined(NDEBUG) && defined(__GNUC__)
/* Nothing. pmesg has been "defined away" in debug.h already. */
#else

void np_debugmsg(int level, char* format, ...) {
#ifdef NDEBUG
/* Empty body, so a good compiler will optimise calls
to pmesg away */
#else

  va_list args;

  if (level > msglevel)
    return;

  va_start(args, format);

  vfprintf(stderr, format, args);
  va_end(args);

#endif /* NDEBUG */
#endif /* NDEBUG && __GNUC__ */
}

#endif  // INCLUDE_NPH_DEBUG_H_