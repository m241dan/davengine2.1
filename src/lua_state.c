#include "mud.h"

const struct luaL_Reg StateLib_m[] = {
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
