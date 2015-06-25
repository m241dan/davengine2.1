#include "mud.h"

const struct luaL_Reg AccountLib_m[] = {
   { NULL, NULL }
};

const struct luaL_Reg AccountLib_f[] = {
   { "new", newAccount },
   { "get", getAccount },
   { "delete", delAccount },
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

   if( lua_gettop( L ) > 2 )
   {
      bug( "%s: bad number of arguments passed.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }
   if( lua_type( L, 1 ) != LUA_TSTRING )
   {
      bug( "%s: arg 1 should be a string.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }
   if( lua_type( L, 2 ) != LUA_TSTRING )
   {
      bug( "%s: arg 2 should be a string.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   account = init_account();
   set_aName( account, lua_tostring( L, 1 ) );
   set_aPasswd( account, NULL, lua_tostring( L, 2 ) );
   new_account( account );
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

   DAVLUACM_ACCOUNT_NONE( account, L );
   delete_account( account );
   return 0;
}

