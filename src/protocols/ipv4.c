#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "define.h"

#define IP4VNAME "ipv4"
#define MYVERSION NAPHEX_VERSION_STR

struct _ipv4_packet {
  u_char *packet;
  size_t size;
};

typedef struct _ipv4_packet IPV4;

static IPV4 *check_ipv4(lua_State *L, int index) {
  IPV4 *ipv4;
  luaL_checktype(L, index, LUA_TUSERDATA);
  ipv4 = (IPV4 *)luaL_checkudata(L, index, IP4VNAME);

  if (ipv4 == NULL) 
    luaL_error(L, "Argument error in ethernet");

  return ipv4;
}

static IPV4 *push_ipv4(lua_State *L) {
  IPV4 *ipv4 = (IPV4 *)lua_newuserdata(L, sizeof(IPV4));
  luaL_getmetatable(L, IP4VNAME);
  lua_setmetatable(L, -2);
  return ipv4;
}

static int ipv4_new(lua_State *L) {
  int argc = lua_gettop(L);
  size_t len;
  
  IPV4 *ipv4 = push_ipv4(L);
  
  if (argc == 1) {
    const char *lpack = luaL_checklstring(L, 1, &len);

    if (len < 20) {
      ipv4->packet = (u_char *) calloc(20, sizeof(u_char));
      ipv4->size = 20;
    }
    else
      ipv4->packet = (u_char *) calloc(len, sizeof(u_char));

    memcpy(ipv4->packet, lpack, len);
    ipv4->size = len;
  }
  else if (argc == 0) {
    ipv4->size = 0;
  }
  else {
    luaL_error(L, "Argument number error in ipv4 packet");
  }

  return 1; 
}

static int ipv4_get(lua_State *L) {
  IPV4 *ipv4 = check_ipv4(L, 1);
  u_char version = (ipv4->packet[0] >> 4) & 0x0f;
  u_char ihl = (ipv4->packet[0]) & 0x0f;
  u_char type_of_service = ipv4->packet[1];

  u_char t_length[2]; 
  t_length[0] = ipv4->packet[2];
  t_length[1] = ipv4->packet[3];
  
  u_int total_length = t_length[1] + t_length[0]*256;

  u_char identification[2];
  identification[0] = ipv4->packet[4];
  identification[1] = ipv4->packet[5];

  u_char flags = (ipv4->packet[6] >> 5) & 0x07;

  u_char offset[2];
  offset[0] = (ipv4->packet[6]) & 0x1F;
  offset[1] = ipv4->packet[7];
  u_int ioffset = offset[1] + offset[0]*256;

  u_char ttl = ipv4->packet[8];
  u_char protocol = ipv4->packet[9];

  u_char checksum[2];
  checksum[0] = ipv4->packet[10];
  checksum[1] = ipv4->packet[11];

  u_char src_ip[4];
  src_ip[0] = ipv4->packet[12];
  src_ip[1] = ipv4->packet[13];
  src_ip[2] = ipv4->packet[14];
  src_ip[3] = ipv4->packet[15];

  u_char dst_ip[4];
  dst_ip[0] = ipv4->packet[16];
  dst_ip[1] = ipv4->packet[17];
  dst_ip[2] = ipv4->packet[18];
  dst_ip[3] = ipv4->packet[19];

  size_t header_length = ihl*4;
  u_char *options;
  size_t options_length = 0;

  u_char *payload = (u_char*) malloc(ipv4->size - header_length);
  int i = 0;

  for(i = header_length; i < ipv4->size; i++) {
    payload[i - header_length] = ipv4->packet[header_length + i];
  }

  lua_newtable(L);

  lua_pushinteger(L, version);
  lua_setfield(L, -2, "version");

  lua_pushinteger(L, ihl);
  lua_setfield(L, -2, "ihl");

  lua_pushinteger(L, type_of_service);
  lua_setfield(L, -2, "type_of_service");

  lua_pushinteger(L, total_length);
  lua_setfield(L, -2, "total_length");

  char str_identification[16];
  sprintf(str_identification, "%02x%02x",
          identification[0], identification[1]);
  lua_pushstring(L, str_identification);
  lua_setfield(L, -2, "identification");

  char str_flags[16];
  sprintf(str_flags, "%02x", flags);
  lua_pushstring(L, str_flags);
  lua_setfield(L, -2, "flags");

  lua_pushinteger(L, ttl);
  lua_setfield(L, -2, "ttl");

  lua_pushinteger(L, protocol);
  lua_setfield(L, -2, "protocol");

  char str_checksum[10];
  sprintf(str_checksum, "%02x%02x", checksum[0], checksum[1]);
  lua_pushstring(L, str_checksum);
  lua_setfield(L, -2, "checksum");

  char str_src_ip[30];
  sprintf(str_src_ip, "%d.%d.%d.%d",
    src_ip[0], src_ip[1], src_ip[2], src_ip[3]);
  lua_pushstring(L, str_src_ip);
  lua_setfield(L, -2, "src_ip");

  char str_dst_ip[30];
  sprintf(str_dst_ip, "%d.%d.%d.%d",
    dst_ip[0], dst_ip[1], dst_ip[2], dst_ip[3]);
  lua_pushstring(L, str_dst_ip);
  lua_setfield(L, -2, "dst_ip");

  
  if (header_length > 20) {
    options_length = header_length - 20;

    options = (u_char *) calloc(options_length, sizeof(u_char));

    int i = 0;
    for(; i < options_length; i++)
      options[i] = ipv4->packet[20 + i];

    lua_pushlstring(L, options, options_length);
    lua_setfield(L, -2, "options");

    free(options);
  }

  lua_pushlstring(L, payload, ipv4->size - header_length);
  lua_setfield(L, -2, "payload");

  
  return 1;
}

static int ipv4_tostring(lua_State *L) {
  IPV4 *ipv4 = check_ipv4(L, 1);
  u_char version = (ipv4->packet[0] >> 4) & 0x0f;
  u_char ihl = (ipv4->packet[0]) & 0x0f;
  u_char type_of_service = ipv4->packet[1];

  u_char t_length[2]; 
  t_length[0] = ipv4->packet[2];
  t_length[1] = ipv4->packet[3];
  
  u_int total_length = t_length[1] + t_length[0]*256;

  u_char identification[2];
  identification[0] = ipv4->packet[4];
  identification[1] = ipv4->packet[5];

  u_char flags = (ipv4->packet[6] >> 5) & 0x07;

  u_char offset[2];
  offset[0] = (ipv4->packet[6]) & 0x1F;
  offset[1] = ipv4->packet[7];
  u_int ioffset = offset[1] + offset[0]*256;

  u_char ttl = ipv4->packet[8];
  u_char protocol = ipv4->packet[9];

  u_char checksum[2];
  checksum[0] = ipv4->packet[10];
  checksum[1] = ipv4->packet[11];

  u_char src_ip[4];
  src_ip[0] = ipv4->packet[12];
  src_ip[1] = ipv4->packet[13];
  src_ip[2] = ipv4->packet[14];
  src_ip[3] = ipv4->packet[15];

  u_char dst_ip[4];
  dst_ip[0] = ipv4->packet[16];
  dst_ip[1] = ipv4->packet[17];
  dst_ip[2] = ipv4->packet[18];
  dst_ip[3] = ipv4->packet[19];

  size_t header_length = ihl*4;
  u_char *options;
  size_t options_length = 0;

  char str_out[2048]= "";
  char str_tmp[256];
  size_t num_tmp;
  num_tmp = sprintf(str_tmp, "Version: %d\n", version);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "header_length: %d bytes\n", ihl*4);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "total_length: %d\n", total_length);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "identification: 0x%02x%02x\n", identification[0], identification[1]);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "flags: 0x%02x\n", flags);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "offset: %d\n", ioffset);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "ttl: %d\n", ttl);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "protocol: %d\n", protocol);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "checksum: 0x%02x%02x\n", checksum[0], checksum[1]);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "src_ip: %d.%d.%d.%d\n",
    src_ip[0], src_ip[1], src_ip[2], src_ip[3]);
  strncat(str_out, str_tmp, num_tmp);
  num_tmp = sprintf(str_tmp, "dst_ip: %d.%d.%d.%d\n",
    dst_ip[0], dst_ip[1], dst_ip[2], dst_ip[3]);
  strncat(str_out, str_tmp, num_tmp);

  if (header_length > 20) {
    options_length = header_length - 20;

    options = (u_char *) calloc(options_length, sizeof(u_char));

    int i = 0;
    num_tmp = sprintf(str_tmp, "options: ");
    strncat(str_out, str_tmp, num_tmp);
    for(; i < options_length; i++) {
      options[i] = ipv4->packet[20 + i];
      num_tmp = sprintf(str_tmp, "%02x", options[i]);
      strncat(str_out, str_tmp, num_tmp);
    }
    num_tmp = sprintf(str_tmp, "\n");
    strncat(str_out, str_tmp, num_tmp);

    free(options);
  }

  lua_pushstring(L, str_out);
  return 1;
}

static int ipv4_gc(lua_State *L) {
  IPV4 *ipv4 = check_ipv4(L, 1);
  free(ipv4->packet);
  return 0;
}

static const luaL_Reg R[] =
{
  { "__gc",                       ipv4_gc },
  { "__tostring",                 ipv4_tostring },
  { "new",                        ipv4_new },
  { "get",                        ipv4_get },
  { NULL,                         NULL }
};

LUALIB_API int luaopen_ipv4(lua_State *L) {
  luaL_newmetatable(L,IP4VNAME);
  luaL_setfuncs(L,R,0);
  lua_pushliteral(L,"version");                  /** version */
  lua_pushliteral(L,MYVERSION);
  lua_settable(L,-3);
  lua_pushliteral(L,"__index");
  lua_pushvalue(L,-2);
  lua_settable(L,-3);
  return 1;
}
