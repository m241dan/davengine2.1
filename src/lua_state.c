#include "mud.h"

const struct luaL_Reg StateLib_m[] = {
   { "setControlFile", state_setControlFile },
   { "control", state_control },
   { "msg", state_message },
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

inline int state_new( lua_state *L )
{
   SOCKET_STATE *state;
   int top = lua_gettop( L );

   if( top != 1 )
   {
      bug( "%s: bad number of arguments passed: State.new( \"control file\" )", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   if( lua_type( L, 1 ) != LUA_TSTRING )
   {
      bug( "%s: control argument passed not of type string.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   state = init_state();
   set_state_control( state, lua_tostring( L, 1 ) );
   lua_pushobj( L, state, SOCKET_STATE );
   return 1;
}

inline int state_setControlFile( lua_state *L )
{
   SOCKET_STATE *state;

   DAVLUACM_SOCKET_BOOL( state, L );

   if( lua_type( L, 2 ) != LUA_TSTRING )
   {
      bug( "%s: argument passed must be of type string.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( !file_exists( lua_tostring( L, 2 ) ) )
   {
      bug( "%s: control file passed does not exist.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   set_state_control( state, lua_tostring( L, 2 ) );
   lua_pushboolean( L, 1 );
   return 1;
}

inline int state_control( lua_State *L )
{
   SOCKET_STATE *state;

   DAVLUACM_SOCKET_NONE( state, L );

   switch( get_meta_type_id( L, 2 )
   {
      default:
         bug( "%s: states cannot control object passed.", __FUNCTION__ );
         return 0;
      case ACCOUNT_TAG:
         set_state_as_account( state, *(ACCOUNT_DATA **)lua_touserdata( L, 2 );
         break;
      case ENTITY_TAG:
         set_state_as_entity( state, *(ENTITY_DATA **)lua_touserdata( L, 2 );
         break;
   }
   return 0;
}

inline int state_message( lua_State *L )
{
   SOCKET_STATE *state;
   int top, buffer_id = 0;

   DAVLUACM_STATE_NONE( state, L );

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

   if( !state->socket )
   {
      bug( "%s: cannot send message, account has no socket.", __FUNCTION__ );
      return 0;
   }

   __text_to_buffer( socket, lua_tostring( L, 2 ), buffer_id );
   return 0;

}
