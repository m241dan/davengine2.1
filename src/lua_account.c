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

inline int getAccount( lua_State *L )
{
   ACCOUNT_DATA *account;

   switch( lua_type( L, -1 ) )
   {
      default:
         bug( "%s: bad arguments passed.", __FUNCTION__ );
         lua_pushnil( L );
         break;
      case LUA_TSTRING:
         if( ( account = get_accountByName( lua_tostring( L, -1 ) ) ) == NULL )
            lua_pushnil( L );
         else
            lua_pushaccount( L, account );
         break;
      case LUA_TNUMBER:
         if( ( account = get_accountByID( lua_tonumber( L, -1 ) ) ) == NULL )
            lua_pushnil( L );
         else
            lua_pushaccount( L, account );
         break;
   }
   return 1;
}

inline int delAccount( lua_State *L )
{
   ACCOUNT_DATA *account;

   if( lua_type( L, -1 ) != LUA_TUSERDATA )
   {
      bug( "%s: bad parameter passed: account userdata only.", __FUNCTION__ );
      return 0;
   }

   if( ( account = *(ACCOUNT_DATA **)luaL_checkudata( L, -1, "Account.meta") ) == NULL )
   {
      bug( "%s: bad parameter passed: non-account userdata passed.", __FUNCTION__ );
      return 0;
   }
   delete_account( account );
   return 0;
}

