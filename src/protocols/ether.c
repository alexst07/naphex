#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define ETHERNAME "ether"
#define MYVERSION "0.0.0"

#define MAX_FRAME_BYTES 1518
#define HEADER_SIZE     14

struct _ether_frame {
  u_char *frame;
  size_t size;
};

typedef struct _ether_frame ETHER_FRAME;

static ETHER_FRAME *to_ether (lua_State *L, int index)
{
  ETHER_FRAME *ether = (ETHER_FRAME *)lua_touserdata(L, index);

  if (ether == NULL)
    luaL_error(L, "Argument error in ethernet");

  return ether;
}

static ETHER_FRAME *check_ether(lua_State *L, int index) {
  ETHER_FRAME *ether;
  luaL_checktype(L, index, LUA_TUSERDATA);
  ether = (ETHER_FRAME *)luaL_checkudata(L, index, ETHERNAME);

  if (ether == NULL) 
    luaL_error(L, "Argument error in ethernet");

  return ether;
}

static ETHER_FRAME *push_ether(lua_State *L) {
  ETHER_FRAME *ether = (ETHER_FRAME *)lua_newuserdata(L, sizeof(ETHER_FRAME));
  luaL_getmetatable(L, ETHERNAME);
  lua_setmetatable(L, -2);
  return ether;
}

static int ether_new(lua_State *L) {
  int argc = lua_gettop(L);
  size_t len;
  
  ETHER_FRAME *ether = push_ether(L);
  
  if (argc == 1) {
    const char *lpack = luaL_checklstring(L, 1, &len);

    if (len < 14) {
      ether->frame = (u_char *) calloc(14, sizeof(u_char));
      ether->size = 14;
    }
    else
      ether->frame = (u_char *) calloc(len, sizeof(u_char));

    memcpy(ether->frame, lpack, len);
    ether->size = len;
  }
  else if (argc == 0) {
    ether->size = 0;
  }
  else {
    luaL_error(L, "Argument number error in ethernet");
  }

  return 1; 
}

static int ether_set_shost(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  const char *mac = luaL_checkstring(L, 2);

  const char* tok = ":";
  char * pch;
  pch = strtok ((char *) mac, tok);

  int i;
  for (i = 0; pch != NULL; i++) {
    u_char hex = (char)strtol(pch, NULL, 16);
    if (i < 6)
      ether->frame[6 + i] = hex;
    else
      break;

    pch = strtok (NULL, tok);
  }

  return 0;
}

static int ether_set_dhost(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  const char *mac = luaL_checkstring(L, 2);
  
  const char* tok = ":";
  char * pch;
  pch = strtok ((char *) mac, tok);
  
  int i;
  for (i = 0; pch != NULL; i++) {
    u_char hex = (char)strtol(pch, NULL, 16);
    if (i < 6)
      ether->frame[i] = hex;
    else
      break;

    pch = strtok (NULL, tok);
  }
  
  return 0;
}

static int ether_set_type(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  const char *type = luaL_checkstring(L, 2);

  u_char hex1[3];
  hex1[0] = type[0];
  hex1[1] = type[1];
  hex1[2] = '\0';

    u_char hex2[3];
  hex2[0] = type[0];
  hex2[1] = type[1];
  hex2[2] = '\0';

  u_char chex1 = (char)strtol(hex1, NULL, 16);
  u_char chex2 = (char)strtol(hex2, NULL, 16);

  ether->frame[12] = chex1;
  ether->frame[13] = chex2;

  return 0;
}

static int ether_get_shost(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  int op = luaL_checkinteger(L, 2);

  char mac[30];

  if (op == 1) {
    sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
      ether->frame[6],
      ether->frame[7],
      ether->frame[8],
      ether->frame[9],
      ether->frame[10],
      ether->frame[11]
    );
  }
  else {
    sprintf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",
      ether->frame[6],
      ether->frame[7],
      ether->frame[8],
      ether->frame[9],
      ether->frame[10],
      ether->frame[11]
    );
  }

  lua_pushstring(L, mac);
  return 1;
}

static int ether_get_dhost(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  int op = luaL_checkinteger(L, 2);
  
  char mac[30];

  if (op == 1) {
    sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
      ether->frame[0],
      ether->frame[1],
      ether->frame[2],
      ether->frame[3],
      ether->frame[4],
      ether->frame[5]
    );
  }
  else {
    sprintf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",
      ether->frame[0],
      ether->frame[1],
      ether->frame[2],
      ether->frame[3],
      ether->frame[4],
      ether->frame[5]
    );
  }
  
  lua_pushstring(L, mac);
  return 1;
}

static int ether_get_type(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  int op = luaL_checkinteger(L, 2);

  char type[5];

  if (op == 1) {
    sprintf(type,"%02x%02x",
      ether->frame[12],
      ether->frame[13]
    );
  }
  else {
    sprintf(type,"%02X%02X",
      ether->frame[12],
      ether->frame[13]
    );
  }

  lua_pushstring(L, type);
  return 1;
}

static int ether_set_payload(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  size_t len;
  const char *payload = luaL_checklstring(L, 2, &len);

  int i;
  ether->frame = (u_char *) realloc(ether->frame, len + 14);

  for(i = 0; i < len; i++)
    ether->frame[14 + i] = (u_char)*(payload + i);

  ether->size = len + 14;

  return 0;
}

static int ether_get_payload(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  int len = ether->size - 14;

  char *payload = (char *) malloc(sizeof(char)*(len));
  int i = 0;
  for(i = 0; i < len; i++)
    *(payload + i) = ether->frame[i + 14];

  lua_pushlstring(L, payload, len);
  free(payload);
  return 1;
}

static int ether_payload_size(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  int len = ether->size - 14;

  lua_pushinteger(L, len);
  return 1;
}

static int ether_set_frame(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  size_t len;
  const char *data = luaL_checklstring(L, 2, &len);
  int start = luaL_checkinteger(L, 3);

  ether->frame = (u_char *) realloc(ether->frame, len + start);
  int i = 0;
  for(; i < len; i++)
    ether->frame[start + i] = (u_char)*(data + i);

  return 0;
}

static int ether_tostring(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  char shost[30], dhost[30], type[5];

  sprintf(dhost, "%02x:%02x:%02x:%02x:%02x:%02x",
          ether->frame[0],
          ether->frame[1],
          ether->frame[2],
          ether->frame[3],
          ether->frame[4],
          ether->frame[5]
  );

  sprintf(shost, "%02x:%02x:%02x:%02x:%02x:%02x",
          ether->frame[6],
          ether->frame[7],
          ether->frame[8],
          ether->frame[9],
          ether->frame[10],
          ether->frame[11]
  );

  sprintf(type,"%02X%02X",
          ether->frame[12],
          ether->frame[13]
  );

  char str[128];
  sprintf(str, "dhost:\t%s\nshost:\t%s\ntype:\t%s",dhost, shost, type);

  lua_pushstring(L, str);
  return 1;
}

static int ether_gc(lua_State *L) {
  ETHER_FRAME *ether = check_ether(L, 1);
  free(ether->frame);
  return 0;
}

static const luaL_Reg R[] =
{
  { "__gc",                       ether_gc },
  { "__tostring",                 ether_tostring },
  { "new",                        ether_new },
  { "set_shost",                  ether_set_shost },
  { "set_dhost",                  ether_set_dhost },
  { "set_type",                   ether_set_type },
  { "get_shost",                  ether_get_shost },
  { "get_dhost",                  ether_get_dhost },
  { "get_type",                   ether_get_type },
  { "set_payload",                ether_set_payload },
  { "get_payload",                ether_get_payload },
  { "payload_size",               ether_payload_size },
  { "set_frame",                  ether_set_frame },
  { NULL,                         NULL }
};

LUALIB_API int luaopen_ether(lua_State *L) {
  luaL_newmetatable(L,ETHERNAME);
  luaL_setfuncs(L,R,0);
  lua_pushliteral(L,"version");                  /** version */
  lua_pushliteral(L,MYVERSION);
  lua_settable(L,-3);
  lua_pushliteral(L,"__index");
  lua_pushvalue(L,-2);
  lua_settable(L,-3);
  return 1;
}