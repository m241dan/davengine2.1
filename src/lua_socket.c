#include "mud.h"

const struct luaL_Reg SocketLib_m[] = {
   { NULL, NULL }
};

const struct luaL_Reg SocketLib_f[] = {
   { "get", getSocket },
   { NULL, NULL }
};

int luaopen_SocketLib( lua_State *L )
{
   luaL_newmetatable( L, "Socket.meta" );

   lua_pushvalue( L, -1 );
   lua_setfield( L, -2, "__index" );

   lua_pushcfunction( L, SocketGC );
   lua_setfield( L, -2, "__gc" );

   luaL_setfuncs( L, SocketLib_m, 0 );

   luaL_newlib( L, SocketLib_f );
   return 1;

}

int SocketGC( lua_State *L )
{
   D_SOCKET **socket;
   socket = (D_SOCKET **)lua_touserdata( L, -1 );
   *socket = NULL;
   return 0;
}

int getSocket( lua_State *L )
{
   D_SOCKET *socket;

   if( lua_type( L, 1 ) != LUA_TUSERDATA )
   {
      bug( "%s: bad argument passed.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   return 0;
}
