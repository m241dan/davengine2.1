#include "mud.h"

const struct luaL_Reg NannyLib_m[] = {
   { NULL, NULL }
};

const struct luaL_Reg NannyLib_f[] = {
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
