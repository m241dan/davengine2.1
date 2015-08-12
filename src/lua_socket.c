#include "mud.h"

const struct luaL_Reg SocketLib_m[] = {
   { "setOutBufWidth", socket_setOutBufWidth },
   { "setState", socket_lsetState },
   { "getOutBufWidth", socket_getOutBufWidth },
   { "getState", socket_getState },
   { "addState", socket_addState },
   { "prev", socket_PrevState },
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

inline int getSocket( lua_State *L )
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
         if( !account->managing_state || !account->managing_state->socket )
         {
            bug( "%s: this account has no socket!", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         }
         lua_pushobj( L, account->managing_state->socket, D_SOCKET );
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
         if( !nanny->managing_state || !nanny->managing_state->socket )
         {
            bug( "%s: this nanny has no socket!", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         }
         lua_pushobj( L, nanny->managing_state->socket, D_SOCKET );
         break;
      }
   }
   return 1;
}

inline int socket_setOutBufWidth( lua_State *L )
{
   D_SOCKET *socket;
   int index, width;

   DAVLUACM_SOCKET_BOOL( socket, L );
   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: bad argument 1 passed, must be of type number.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( lua_type( L, 3 ) != LUA_TNUMBER )
   {
      bug( "%s: bad argument 2 passed,must be of type number.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( ( index = lua_tonumber( L, 2 ) ) < 0 || index >= OUT_BUFS )
   {
      bug( "%s: bad index passed, must be between 0 and %d.", __FUNCTION__, ( OUT_BUFS - 1 ) );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( ( width = lua_tonumber( L, 3 ) ) < 0 )
   {
      bug( "%s: bad width passed, must be greater than 0.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   set_buffer_width( socket->outbuf[index], width );
   lua_pushboolean( L, 1 );
   return 1;
}

inline int socket_lsetState( lua_State *L )
{
   D_SOCKET *socket;
   int state_to_set;

   DAVLUACM_SOCKET_NONE( socket, L );
   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: bad arguments passed as parameter, must be number.", __FUNCTION__ );
      return 0;
   }

   if( ( state_to_set = lua_tonumber( L, 2 ) ) < 0 || state_to_set >= MAX_STATE )
   {
      bug( "%s: state argument passed outside of range, 0-9 only.", __FUNCTION__ );
      return 0;
   }
   socket_setState( socket, state_to_set );
   return 0;
}

inline int socket_getOutBufWidth( lua_State *L )
{
   D_SOCKET *socket;
   int buf_index;

   DAVLUACM_SOCKET_NIL( socket, L );

   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: argument one needs to be of type number.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   buf_index = lua_tonumber( L, 2 );
   if( buf_index < 0 || buf_index >= OUT_BUFS )
   {
      bug( "%s: The buffer indexs range from 0 to %s.", __FUNCTION__, (OUT_BUFS-1) );
      lua_pushnil( L );
      return 1;
   }
   lua_pushnumber( L, socket->outbuf[buf_index]->width );
   return 1;
}

inline int socket_getState( lua_State *L )
{
   D_SOCKET *socket;

   DAVLUACM_SOCKET_NIL( socket, L );
   lua_pushnumber( L, socket->state_index );
   return 1;
}

inline int socket_addState( lua_State *L )
{

}

inline int socket_PrevState( lua_State *L )
{
   D_SOCKET *socket;

   DAVLUACM_SOCKET_NONE( socket, L );
   socket_setState( socket, socket->previous_index );
   return 0;
}
