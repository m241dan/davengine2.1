#include "mud.h"

const struct luaL_Reg StateLib_m[] = {
   { NULL, NULL }
};

const struct luaL_Reg StateLib_f[] = {
   { NULL, NULL }
};

int luaopen_StateLib( lua_State *L )
{
   luaL_newmetatable( L, "State.meta" );

   lua_pushvalue( L, -1 );
   lua_setfield( L, -2, "__index" );

   lua_pushcfunction( L, StateGC );
   lua_setfield( L, -2, "__gc" );

   luaL_setfuncs( L, StateLib_m, 0 );

   luaL_newlib( L, StateLib_f );
   return 1;
}

int StateGC( lua_State *L )
{
   SOCKET_STATE **state;
   state = (SOCKET_STATE **)lua_touserdata( L, -1 );
   *state = NULL;
   return 0;
}

