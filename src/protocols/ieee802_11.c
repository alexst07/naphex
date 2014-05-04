#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "define.h"

#define IEEE082_11NAME "ieee802_11"
#define MYVERSION       NAPHEX_VERSION_STR

struct _ieee802_11 {
  u_char *frame;
  size_t size;
};

typedef struct _ieee802_11 WLAN_FRAME;

static WLAN_FRAME *check_ieee802_11(lua_State *L, int index) {
  WLAN_FRAME *wlan;
  luaL_checktype(L, index, LUA_TUSERDATA);
  wlan = (WLAN_FRAME *)luaL_checkudata(L, index, IEEE082_11NAME);
  
  if (wlan == NULL) 
    luaL_error(L, "Argument error in ethernet");
  
  return wlan;
}

static WLAN_FRAME *push_ieee802_11(lua_State *L) {
  WLAN_FRAME *wlan = (WLAN_FRAME *)lua_newuserdata(L, sizeof(WLAN_FRAME));
  luaL_getmetatable(L, IEEE082_11NAME);
  lua_setmetatable(L, -2);
  return wlan;
}

static int ieee802_11_new(lua_State *L) {
  int argc = lua_gettop(L);
  size_t len;
  
  WLAN_FRAME *wlan = push_ieee802_11(L);
  
  if (argc == 1) {
    const char *lpack = luaL_checklstring(L, 1, &len);

    if (len < 40) {
      wlan->frame = (u_char *) calloc(40, sizeof(u_char));
      wlan->size = 40;
    }
    else
      wlan->frame = (u_char *) calloc(len, sizeof(u_char));

    memcpy(wlan->frame, lpack, len);
    wlan->size = len;
  }
  else if (argc == 0) {
    wlan->size = 0;
  }
  else {
    luaL_error(L, "Argument number error in ieee802_11");
  }

  return 1; 
}

static void get_default(lua_State *L, u_char *frame, size_t len) {
  lua_pushlstring(L, frame, len);
  lua_setfield(L, -2, "frame");
}

static void management(lua_State *L, u_char *frame, size_t len) {
  u_char subtype = (frame[0] >> 4) & 0x0f;

  // specify the ht field
  u_char oreder = (frame[1] >> 7) & 0x01;

  char address1[30];
  sprintf(address1,"%02x:%02x:%02x:%02x:%02x:%02x",
          frame[4],
          frame[5],
          frame[6],
          frame[7],
          frame[8],
          frame[9]
  );
  
  lua_pushstring(L, address1);
  lua_setfield(L, -2, "address1");
  
  char address2[30];
  sprintf(address2,"%02x:%02x:%02x:%02x:%02x:%02x",
          frame[10],
          frame[11],
          frame[12],
          frame[13],
          frame[14],
          frame[15]
  );
  
  lua_pushstring(L, address2);
  lua_setfield(L, -2, "address2");
  
  char address3[30];
  sprintf(address3,"%02x:%02x:%02x:%02x:%02x:%02x",
          frame[16],
          frame[17],
          frame[18],
          frame[19],
          frame[20],
          frame[21]
  );
  
  lua_pushstring(L, address3);
  lua_setfield(L, -2, "address3");
  
  char seq_control[10];
  sprintf(seq_control,"%02x%02x",
          frame[22],
          frame[23]
  );
  
  lua_pushstring(L, seq_control);
  lua_setfield(L, -2, "seq_control");
  
  u_int shift = 0;

  if (oreder) {
    char ht_control[20];
    sprintf(ht_control,"%02x%02x%02x%02x",
            frame[24],
            frame[25],
            frame[26],
            frame[27]
    );
    
    lua_pushstring(L, ht_control);
    lua_setfield(L, -2, "ht_control");
    
    shift += 4;
  }

  u_char *payload = (u_char*) malloc(len);
  int i = 0;
  for(i = 24 + shift; i < len; i++) {
    payload[i - (24 + shift)] = frame[i];
  }
  
  lua_pushlstring(L, payload, len - (24 + shift));
  lua_setfield(L, -2, "payload");
}

static void control(lua_State *L, u_char *frame, size_t len) {
  u_char subtype = (frame[0] >> 4) & 0x0f;

  if ((subtype == 0x0b) ||      // RTS
      (subtype == 0x0a) ||      // PS-Poll
      (subtype == 0x0e) ||      // CF-End
      (subtype == 0x0f)) {      // CF-End + CF-Ack
    char ra[30];
    sprintf(ra,"%02x:%02x:%02x:%02x:%02x:%02x",
            frame[4],
            frame[5],
            frame[6],
            frame[7],
            frame[8],
            frame[9]
    );
    
    lua_pushstring(L, ra);
    lua_setfield(L, -2, "ra");

    char ta[30];
    sprintf(ta,"%02x:%02x:%02x:%02x:%02x:%02x",
            frame[10],
            frame[11],
            frame[12],
            frame[13],
            frame[14],
            frame[15]
    );
    
    lua_pushstring(L, ta);
    lua_setfield(L, -2, "ta");
  }
  else if ((subtype == 0x0c) || // CTS
           (subtype == 0x0d)) { // ACK
    char ra[30];
    sprintf(ra,"%02x:%02x:%02x:%02x:%02x:%02x",
            frame[4],
            frame[5],
            frame[6],
            frame[7],
            frame[8],
            frame[9]
    );
    
    lua_pushstring(L, ra);
    lua_setfield(L, -2, "ra");
  }
  else if ((subtype == 0x08) || //Block Ack Request
           (subtype == 0x08)) { //Block Ack
    char ra[30];
    sprintf(ra,"%02x:%02x:%02x:%02x:%02x:%02x",
            frame[4],
            frame[5],
            frame[6],
            frame[7],
            frame[8],
            frame[9]
    );
    
    lua_pushstring(L, ra);
    lua_setfield(L, -2, "ra");

    char ta[30];
    sprintf(ta,"%02x:%02x:%02x:%02x:%02x:%02x",
            frame[10],
            frame[11],
            frame[12],
            frame[13],
            frame[14],
            frame[15]
    );
    
    lua_pushstring(L, ta);
    lua_setfield(L, -2, "ta");

    char bar_ctrl[10];
    sprintf(ta,"%02x%02x",
            frame[16],
            frame[17]
    );
    
    lua_pushstring(L, bar_ctrl);
    lua_setfield(L, -2, "bar_ctrl");

    u_char *payload = (u_char*) malloc(len);
    int i = 0;
    for(i = 18; i < len; i++) {
      payload[i - 18] = frame[i];
    }

    lua_pushlstring(L, payload, len - 18);
    lua_setfield(L, -2, "payload");
  }
  else {
    lua_pushlstring(L, frame, len);
    lua_setfield(L, -2, "frame");
  }
      
}

static void data(lua_State *L, u_char *frame, size_t len) {
  u_char subtype = (frame[0] >> 4) & 0x0f;

  // specify the ht field
  u_char oreder = (frame[1] >> 7) & 0x01;

  // specify the qos field
  u_char qos = (frame[0] >> 7) & 0x01;

  // specify the address4
  u_char to_ds = frame[1] & 0x01;
  u_char from_ds = (frame[1] >> 1) & 0x01; 

  char address1[30];
  sprintf(address1,"%02x:%02x:%02x:%02x:%02x:%02x",
          frame[4],
          frame[5],
          frame[6],
          frame[7],
          frame[8],
          frame[9]
  );

  lua_pushstring(L, address1);
  lua_setfield(L, -2, "address1");

  char address2[30];
  sprintf(address2,"%02x:%02x:%02x:%02x:%02x:%02x",
          frame[10],
          frame[11],
          frame[12],
          frame[13],
          frame[14],
          frame[15]
  );

  lua_pushstring(L, address2);
  lua_setfield(L, -2, "address2");

  char address3[30];
  sprintf(address3,"%02x:%02x:%02x:%02x:%02x:%02x",
          frame[16],
          frame[17],
          frame[18],
          frame[19],
          frame[20],
          frame[21]
  );

  lua_pushstring(L, address3);
  lua_setfield(L, -2, "address3");

  char seq_control[10];
  sprintf(seq_control,"%02x%02x",
          frame[22],
          frame[23]
  );

  lua_pushstring(L, seq_control);
  lua_setfield(L, -2, "seq_control");

  u_int shift = 0;

  if (to_ds & from_ds) {
    char address4[30];
    sprintf(address4,"%02x:%02x:%02x:%02x:%02x:%02x",
            frame[24],
            frame[25],
            frame[26],
            frame[27],
            frame[28],
            frame[29]
    );

    lua_pushstring(L, address4);
    lua_setfield(L, -2, "address4");

    shift += 6;
  }

  if (qos) {
    char qos_field[10];
    sprintf(qos_field,"%02x%02x",
            frame[24 + shift],
            frame[25 + shift]
    );

    lua_pushstring(L, qos_field);
    lua_setfield(L, -2, "qos");

    shift += 2;
  }

  if (oreder) {
    char ht_control[20];
    sprintf(ht_control,"%02x%02x%02x%02x",
            frame[24 + shift],
            frame[25 + shift],
            frame[26 + shift],
            frame[27 + shift]
    );

    lua_pushstring(L, ht_control);
    lua_setfield(L, -2, "ht_control");

    shift += 4;
  }

  u_char *payload = (u_char*) malloc(len);
  int i = 0;
  for(i = 24 + shift; i < len; i++) {
    payload[i - (24 + shift)] = frame[i];
  }

  lua_pushlstring(L, payload, len - (24 + shift));
  lua_setfield(L, -2, "payload");
}

static int ieee802_11_get(lua_State *L) {
  WLAN_FRAME *wlan = check_ieee802_11(L, 1);
  u_char type = (wlan->frame[0] >> 2) & 0x03;
  u_char subtype = (wlan->frame[0] >> 4) & 0x0f;
  u_char byte = wlan->frame[0];
  u_char version = (wlan->frame[0]) & 0x03;
  u_char to_ds = (wlan->frame[1]) & 0x01;
  u_char from_ds = (wlan->frame[1] >> 1) & 0x01;
  u_char more_frag = (wlan->frame[1] >> 2) & 0x01;
  u_char retry = (wlan->frame[1] >> 3) & 0x01;
  u_char power_mngmnt = (wlan->frame[1] >> 4) & 0x01;
  u_char more_data = (wlan->frame[1] >> 5) & 0x01;
  u_char protected_frame = (wlan->frame[1] >> 6) & 0x01;
  u_char oreder = (wlan->frame[1] >> 7) & 0x01;

  lua_newtable(L);

  lua_pushinteger(L, version);
  lua_setfield(L, -2, "version");

  lua_pushinteger(L, type);
  lua_setfield(L, -2, "type");

  lua_pushinteger(L, subtype);
  lua_setfield(L, -2, "subtype");

  lua_pushboolean(L, to_ds);
  lua_setfield(L, -2, "to_ds");

  lua_pushboolean(L, from_ds);
  lua_setfield(L, -2, "from_ds");

  lua_pushboolean(L, more_frag);
  lua_setfield(L, -2, "more_fragments");

  lua_pushboolean(L, retry);
  lua_setfield(L, -2, "retry");

  lua_pushboolean(L, power_mngmnt);
  lua_setfield(L, -2, "power_management");

  lua_pushboolean(L, more_data);
  lua_setfield(L, -2, "more_data");

  lua_pushboolean(L, protected_frame);
  lua_setfield(L, -2, "protected_frame");

  lua_pushboolean(L, oreder);
  lua_setfield(L, -2, "oreder");
  
  switch(type) {
    case 0x00:
      management(L, wlan->frame, wlan->size);
      break;

    case 0x01:
      control(L, wlan->frame, wlan->size);
      break;

    case 0x02:
      data(L, wlan->frame, wlan->size);
      break;

    case 0x03:
      reserved(L, wlan->frame, wlan->size);
      break;
  }
  printf("byte: %02x\ntype: %02x\nsubtype: %02x \n\n", wlan->frame[0], type, subtype);
}

static int ieee802_11_gc(lua_State *L) {
  WLAN_FRAME *wlan = check_ieee802_11(L, 1);
  free(wlan->frame);
  return 0;
}

static const luaL_Reg R[] =
{
  { "__gc",                       ieee802_11_gc },
  { "new",                        ieee802_11_new },
  { "get",                        ieee802_11_get },
  { NULL,                         NULL }
};

LUALIB_API int luaopen_ieee802_11(lua_State *L) {
  luaL_newmetatable(L,IEEE082_11NAME);
  luaL_setfuncs(L,R,0);
  lua_pushliteral(L,"version");                  /** version */
  lua_pushliteral(L,MYVERSION);
  lua_settable(L,-3);
  lua_pushliteral(L,"__index");
  lua_pushvalue(L,-2);
  lua_settable(L,-3);
  return 1;
}