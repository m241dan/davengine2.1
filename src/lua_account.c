#include "mud.h"

const struct luaL_Reg AccountLib_m[] = {
   { NULL, NULL }
};

const struct luaL_Reg AccountLib_f[] = {
   { "new", newAccount },
   { NULL, NULL }
};

int luaopen_AccountLib( lua_State *L )
{
   luaL_newmetatable( L, "Account.meta" );

   lua_pushvalue( L, -1 );
   lua_setfield( L, -2, "__index" );

   lua_pushcfunction( L, AccountGC );
   lua_setfield( L, -2, "__gc" );

   luaL_setfuncs( L, AccountLib_m, 0 );

   luaL_newlib( L, AccountLib_f );
   return 1;
}

int AccountGC( lua_State *L )
{
   ACCOUNT_DATA **account;
   account = (ACCOUNT_DATA **)lua_touserdata( L, -1 );
   *account = NULL;
   return 0;
}

inline int newAccount( lua_State *L )
{
   ACCOUNT_DATA *account;

   account = init_account();
   lua_pushaccount( L, account );
   return 1;
}
