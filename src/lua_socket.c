#include "mud.h"

const struct luaL_Reg SocketLib_m[] = {
   { "setState", socket_lsetState },
   { "getState", socket_getState },
   { "control", socket_Control },
   { "msg", socket_Message },
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

int socket_lsetState( lua_State *L )
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
   socket->state_index = state_to_set;
   return 0;
}

int socket_getState( lua_State *L )
{
   D_SOCKET *socket;

   DAVLUACM_SOCKET_NIL( socket, L );
   lua_pushnumber( L, socket->state_index );
   return 1;
}

int socket_Control( lua_State *L )
{
   D_SOCKET *socket;
   SOCKET_STATE *state;
   int control_index;

   DAVLUACM_SOCKET_NIL( socket, L );

   switch( get_meta_type_id( L, 2 ) )
   {
      default:
         bug( "%s: this user data is not controllable by a socket", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      case ACCOUNT_TAG:
      {
         ACCOUNT_DATA *account;
         if( ( account = *(ACCOUNT_DATA **)lua_touserdata( L, 2 ) ) == NULL )
         {
            bug( "%s: the account box is empty.", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         }
         state = init_state();
         set_state_as_account( state, account );
         set_state_control( state, GLOBAL_ACCOUNT_CONTROL );
         socket->account = account;
         break;
      }
      case NANNY_TAG:
      {
         NANNY_DATA *nanny;
         if( ( nanny = *(NANNY_DATA **)lua_touserdata( L, 2 ) ) == NULL )
         {
            bug( "%s: the nanny box is empty.", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         }
         state = init_state();
         set_state_as_nanny( state, nanny );
         set_state_control( state, nanny->lua_path );
         break;
      }

   }
   if( ( control_index = socket_addState( socket, state ) ) == -1 )
   {
      bug( "%s: no available states, could not add another.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }
   lua_pushnumber( L, control_index );
   return 1;
}

int socket_Message( lua_State *L )
{
   D_SOCKET *socket;
   int top, buffer_id = 0;

   DAVLUACM_SOCKET_NONE( socket, L );

   if( ( top = lua_gettop( L ) ) < 2 )
   {
      bug( "%s: bad number of arguments passed: msg( message, buffer_id )", __FUNCTION__ );
      return 0;
   }

   if( lua_type( L, 2 ) != LUA_TSTRING )
   {
      bug( "%s: message must be of type string.", __FUNCTION__ );
      return 0;
   }

   if( top == 3 )
   {
      if( lua_type( L, 3 ) != LUA_TNUMBER )
      {
         bug( "%s: buffer_id must be of type number.", __FUNCTION__ );
         return 0;
      }
      buffer_id = lua_tonumber( L, 3 );
   }

   __text_to_buffer( socket, lua_tostring( L, 2 ), buffer_id );
   return 0;
}