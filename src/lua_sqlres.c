#include "mud.h"

int luaopen_SqlResLib( lua_State *L )
{
   luaL_newmetatable( L, "SqlRes.meta" );

   lua_pushcfunction( L, SqlResGC );
   lua_setfield( L, -2, "__gc" );

   return 0;
}

int SqlResGC( lua_State *L )
{
   MYSQL_RES *result;
   result = *(MYSQL_RES **)lua_touserdata( L, 1 );
   mysql_free_result( result );
   return 0;
}

