#include "mud.h"

const struct luaL_Reg NannyLib_m[] = {
   { "setSocket", nanny_setSocket },
   { "getState", nanny_getState },
   { NULL, NULL }
};

const struct luaL_Reg NannyLib_f[] = {
   { "new", newNanny },
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
   D_SOCKET *socket;

   DAVLUACM_NANNY_NONE( nanny, L );
   if( ( socket = (D_SOCKET *)check_meta( L, 2, "Socket.meta" ) ) == NULL )
   {
      bug( "%s: setSocket must receive Socket.meta userdata.", __FUNCTION__ );
      return 0;
   }
   set_nSocket( nanny, socket );
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

