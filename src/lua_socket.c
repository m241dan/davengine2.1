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
   if( lua_type( L, 1 ) != LUA_TUSERDATA )
   {
      bug( "%s: bad argument passed.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   switch( get_meta_type_id( L, 1 ) )
   {
      default:
         bug( "%s: this user data does not have a direct link to a socket.", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      case ACCOUNT_TAG:
      {
         ACCOUNT_DATA *account;
         if( ( account = *(ACCOUNT_DATA **)lua_touserdata( L, 1 ) ) == NULL )
         {
            bug( "%s: the account box is empty.", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         }
         if( !account->socket )
         {
            bug( "%s: this account has no socket!", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         }
         lua_pushobj( L, account->socket, D_SOCKET );
         break;
      }
      case NANNY_TAG:
      {
         NANNY_DATA *nanny;
         if( ( nanny = *(NANNY_DATA **)lua_touserdata( L, 1 ) ) == NULL )
         {
            bug( "%s: the nanny box is empty.", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         }
         if( !nanny->socket )
         {
            bug( "%s: this nanny has no socket!", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         }
         lua_pushobj( L, nanny->socket, D_SOCKET );
         break;
      }
   }
   return 1;
}
