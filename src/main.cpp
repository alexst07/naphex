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

#include <iostream>
#include <string>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include "lualibs.h"
#include "lua_utils.h"
#include "args.h"
#include "debug.h"
#include "define.h"
#include "config_imp.h"

void print_usage() {
  std::cout << "version: " << NAPHEX_VERSION_STR << std::endl;
  std::cout <<
  "usage: naphex [--version] [--file=<path>] [--lfile=<path>] [args]"
  << std::endl << std::endl;

  std::cout << "options:" << std::endl;
  std::cout
  << "  -v,  --version    output version information and exit" << std::endl
  << "  -f,  --file       specify the main lua file" << std::endl
  << "  -l,  --lfile      specify the function lua file" << std::endl
  << "  -h,  --help       display this help and exit" << std::endl;
}

int main(int argc, char **argv) {
  lua_State *L;
  L = luaL_newstate();
  int opt;
  bool use_lf = false;
  bool use_f = false;
  std::string file;

  struct option longopts[] = {
    {"version", 0,      NULL,   'v'},
    {"lffile",  0,      NULL,   'l'},
    {"help",    0,      NULL,   'h'},
    {"file",    0,      NULL,   'f'},
    {0,         0,      0,       0}};


  while ((opt = getopt_long (argc, argv, "vflh:", longopts, NULL)) != -1) {
    switch(opt) {
      case 'v':
        std::cout << "version: " << NAPHEX_VERSION_STR << std::endl;
        exit(0);
        break;

      case 'h':
        print_usage();
        exit(0);
        break;

      case 'l':
        use_lf = true;
        break;

      case 'f':
        use_f = true;
        break;

      default:
        std::cout << "unknown option" << std::endl;
        print_usage();
        exit(0);
    }
  }

  if (argc < 2)  {
    print_usage();
    exit(EXIT_FAILURE);
  }

  if (use_f && (argc > 1))
    file = argv[optind++];

  if (use_lf && (argc > 2))
    set_lua_filename(argv[optind++]);

  for(; optind < argc; optind++) {
    Debug(5) << "Argument" << optind << ":" << argv[optind];
    args::insert(argv[optind]);
  }

  luaL_openlibs(L);

  luaL_requiref(L, "monitor", &luaopen_monlib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "wait_filter", &luaopen_wflib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "thread", &luaopen_threadlib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "ondevice", &luaopen_on_devlib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "offdevice", &luaopen_off_devlib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "packet", &luaopen_packetlib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "channel", &luaopen_channellib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "args", &luaopen_argslib, 1);
  lua_pop(L, 1);

  // Call the function to list all devices
  luaopen_ldevfunc(L);

  config *conf = new config_imp(L);

  if (!conf->load_protocols()) {
    std::cerr << "WARNING: Can't load protocols libs" << std::endl;
    exit(-1);
  }

  delete conf;

  if (luaL_loadfile(L, file.c_str()) != 0)  {
    fprintf(stderr, "Could not load: %s\n", file.c_str());
    exit(EXIT_FAILURE);
  }

  if (lua_pcall(L, 0, 0, 0) != 0)  {
    fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    exit(EXIT_FAILURE);
  }

  // Exit when user digit Ctrl+D on Linux
  while(getchar() != EOF);

  lua_close(L);

  return 0;
}
