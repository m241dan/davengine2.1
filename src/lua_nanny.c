#include "mud.h"

const struct luaL_Reg NannyLib_m[] = {
   { "setState", nanny_setState },
   { "setSocket", nanny_setSocket },
   { "getState", nanny_getState },
   { "nextState", nanny_stateNext },
   { "prevState", nanny_statePrev },
   { "msg", nanny_Message },
   { "finish", nanny_Finish },
   { NULL, NULL }
};

const struct luaL_Reg NannyLib_f[] = {
   { "new", newNanny },
   { "get", getNanny },
   { NULL, NULL }
};

int luaopen_NannyLib( lua_State *L )
{
   luaL_newmetatable( L, "Nanny.meta" );

   lua_pushvalue( L, -1 );
   lua_setfield( L, -2, "__index" );

   lua_pushcfunction( L, NannyGC );
   lua_setfield( L, -2, "__gc" );

   luaL_setfuncs( L, NannyLib_m, 0 );

   luaL_newlib( L, NannyLib_f );
   return 1;
}

int NannyGC( lua_State *L )
{
   NANNY_DATA **nanny;
   nanny = (NANNY_DATA **)lua_touserdata( L, -1 );
   *nanny = NULL;
   return 0;
}

int newNanny( lua_State *L )
{
   NANNY_DATA *nanny;

   if( lua_type( L, 1 ) != LUA_TSTRING )
   {
      bug( "%s: must pass the lua script for the nanny.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   nanny = init_nanny();
   set_nPath( nanny, lua_tostring( L, 1 ) );
   if( !load_nanny( nanny ) )
   {
      bug( "%s: could not load the nanny.", __FUNCTION__ );
      free_nanny( nanny );
      lua_pushnil( L );
      return 1;
   }

   lua_pushobj( L, nanny, NANNY_DATA );
   return 1;
}

int getNanny( lua_State *L )
{
/*
   NANNY_DATA *nanny;

   if( lua_type( L, 1 ) != LUA_TUSERDATA )
   {
      bug( "%s: bad argument 1 passed as parameter, needs to be user data.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }
   switch( get_meta_type_id( L, 1 ) )
   {
      default:
         bug( "%s: bad userdata type passed.", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      case SOCKET_TAG:
      {
         D_SOCKET *socket;

         if( ( socket = *(D_SOCKET **)lua_touserdata( L, 1 ) ) == NULL )
         {
            bug( "%s: the socket box is empty!", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         }
         
      }
   }
*/
   return 0;
}

/* getters */
int nanny_getState( lua_State *L )
{
   NANNY_DATA *nanny;

   DAVLUACM_NANNY_NIL( nanny, L );
   lua_pushnumber( L, nanny->state );
   return 1;
}

/* setters */
int nanny_setSocket( lua_State *L )
{
   NANNY_DATA *nanny;
   SOCKET_STATE *state;
   D_SOCKET *socket;

   DAVLUACM_NANNY_NONE( nanny, L );
   if( ( socket = (D_SOCKET *)check_meta( L, 2, "Socket.meta" ) ) == NULL )
   {
      bug( "%s: setSocket must receive Socket.meta userdata.", __FUNCTION__ );
      return 0;
   }
   state = init_state();
   set_state_as_nanny( state, nanny );
   set_state_control( state, nanny->lua_path );
   nanny->managing_state = state;
   return 0;
}

int nanny_setState( lua_State *L )
{
   NANNY_DATA *nanny;

   DAVLUACM_NANNY_BOOL( nanny, L );
   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: nanny states can only be setto numbers", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   set_nState( nanny, lua_tonumber( L, 2 ) );
   lua_pushboolean( L, 1 );
   return 1;
}

/* utility */
int nanny_stateNext( lua_State *L )
{
   NANNY_DATA *nanny;

   DAVLUACM_NANNY_BOOL( nanny, L );
   state_nNext( nanny );
   lua_pushboolean( L, 1 );
   return 1;

}

int nanny_statePrev( lua_State *L )
{
   NANNY_DATA *nanny;

   DAVLUACM_NANNY_BOOL( nanny, L );
   state_nPrev( nanny );
   lua_pushboolean( L, 1 );
   return 1;
}

int nanny_Message( lua_State *L )
{
   NANNY_DATA *nanny;
   D_SOCKET *socket;
   int top, buffer_id = 0;

   DAVLUACM_NANNY_NONE( nanny, L );

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

   if( !nanny->managing_state || ( socket = nanny->managing_state->socket ) == NULL )
   {
      bug( "%s: cannot send message, nanny has no socket.", __FUNCTION__ );
      return 0;
   }

   __text_to_buffer( socket, lua_tostring( L, 2 ), buffer_id );
   return 0;
}

int nanny_Finish( lua_State *L )
{
   NANNY_DATA *nanny;
   int ret, top = lua_gettop( lua_handle );

   DAVLUACM_NANNY_NONE( nanny, L );
   prep_stack( nanny->lua_path, "onNannyFinish" );
   lua_pushobj( lua_handle, nanny, NANNY_DATA );
   if( ( ret = lua_pcall( lua_handle, 1, LUA_MULTRET, 0 ) ) )
      bug( "%s: ret %d: path %s\r\n - error message: %s", __FUNCTION__, ret, nanny->lua_path, lua_tostring( lua_handle, -1 ) );
   free_nanny( nanny );
   lua_settop( lua_handle, top );
   return 0;
}
