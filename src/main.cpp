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

int main(int argc, char **argv) {
  lua_State *L;
  L = luaL_newstate();
  int opt;
  int index;
  bool use_lf = false;
  std::string file;

  struct option longopts[] = {
    {"version", 0, NULL, 'v'},
    {"lffile", 0, NULL, 'l'},
    {"help", 0, NULL, 'h'},
    {0,0,0,0}};


  while ((opt = getopt_long (argc, argv, "vflh:", longopts, NULL)) != -1) {
    switch(opt) {
      case 'v':
	std::cout << "version" << std::endl;
	break;

      case 'h':
	std::cout << "help" << std::endl;
	break;
      
      case 'l':
	use_lf = true;
	break;

      default:
	std::cout << "unknown option" << std::endl;
	return 0;
    }
  }

  file = argv[optind++];

  if (use_lf && (argc > 2))
    set_lua_filename(argv[optind++]);
    
  for(; optind < argc; optind++)
    printf("argument: %s\n", argv[optind]);

  luaL_openlibs(L);
  
  luaL_requiref(L, "monitor", &luaopen_monlib, 1);
  lua_pop(L, 1); // requiref leaves the library table on the stack

  luaL_requiref(L, "wait_filter", &luaopen_wflib, 1);
  lua_pop(L, 1); // requiref leaves the library table on the stack

  luaL_requiref(L, "thread", &luaopen_threadlib, 1);
  lua_pop(L, 1); // requiref leaves the library table on the stack

  luaL_requiref(L, "ondevice", &luaopen_on_devlib, 1);
  lua_pop(L, 1); // requiref leaves the library table on the stack

  luaL_requiref(L, "offdevice", &luaopen_off_devlib, 1);
  lua_pop(L, 1);  

  luaL_requiref(L, "packet", &luaopen_packetlib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "channel", &luaopen_channellib, 1);
  lua_pop(L, 1);

  luaL_requiref(L, "args", &luaopen_argslib, 1);
  lua_pop(L, 1);
  
  if(argc < 2)  {
    fprintf(stderr, "File expected\n");
    exit(EXIT_FAILURE);
  }

  if(luaL_loadfile(L, argv[1]) != 0)  {
    fprintf(stderr, "Could not load: %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  
  if(lua_pcall(L, 0, 0, 0) != 0)  {
    fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    exit(EXIT_FAILURE);
  }

  // Exit when user digit Ctrl+D on Linux
  while(getchar() != EOF);
  
  lua_close(L);
  

  return 0;
}
