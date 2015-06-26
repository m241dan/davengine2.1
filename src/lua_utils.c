/* Davengine Lua C API utilities written by Davenge */

#include "mud.h"

/* creator */

void init_lua_handle( void )
{
   if( ( lua_handle = luaL_newstate() ) == NULL )
   {
      bug( "Could not initialize the Lua handle." );
      exit(1);
   }
   log_string( "Loading Default Lua Libs" );
   luaL_openlibs( lua_handle );

   log_string( "Loading Account Lua Lub" );
   luaL_requiref( lua_handle, "Account", luaopen_AccountLib, 1 );
   lua_pop( lua_handle, -1 );
}

void load_lua_server_config_script( void )
{
   int ret, top = lua_gettop( lua_handle );

   if( luaL_loadfile( lua_handle, SERVER_SCRIPT ) || ( ret = lua_pcall( lua_handle, 0, 0, 0 ) ) )
   {
      bug( "%s: could not load server settings script.\r\n - Ret: %d - %s", __FUNCTION__, ret, lua_tostring( lua_handle, -1 ) );
      return;
   }
   /* server configs */
   lua_getglobal( lua_handle, "mudport" );
   MUD_PORT = lua_tonumber( lua_handle, -1 ) ;
   lua_getglobal( lua_handle, "mud_name" );
   MUD_NAME = new_string( "%s", lua_tostring( lua_handle, -1 ) );

   lua_getglobal( lua_handle, "db_name" );
   DB_NAME = new_string( "%s", lua_tostring( lua_handle, -1 ) );
   lua_getglobal( lua_handle, "db_login" );
   DB_LOGIN = new_string( "%s", lua_tostring( lua_handle, -1 ) );
   lua_getglobal( lua_handle, "db_pass" );
   DB_PASSWORD = new_string( "%s", lua_tostring( lua_handle, -1 ) );
   lua_getglobal( lua_handle, "db_addr" );
   DB_ADDR = new_string( "%s", lua_tostring( lua_handle, -1 ) );

   lua_settop( lua_handle, top );
}


/*
void load_lua_database_config_scripts( void )
{
   int ret, top = lua_gettop( lua_handle );

   if( luaL_loadfile( lua_handle, 
   lua_getglobal( lua_handle, "db_name" );
   DB_NAME =  new_string( "%s", lua_tostring( lua_handle, -1 ) );
   lua_getglobal( lua_handle, "db_addr" );
   DB_ADDR = new_string( "%s", lua_tostring( lua_handle, -1 ) );
   lua_getglobal( lua_handle, "db_login" );
   DB_LOGIN = new_string( "%s", lua_tostring( lua_handle, -1 ) );
   lua_getglobal( lua_handle, "db_pass" );
   DB_PASSWORD = new_string( "%s", lua_tostring( lua_handle, -1 ) );

   lua_settop( lua_handle, top );
}

*/

void lua_pushaccount( lua_State *L, ACCOUNT_DATA *account )
{
   ACCOUNT_DATA **box;

   if( !account )
   {
      bug( "%s: attempting to push NULL account; pushed a nil instead.", __FUNCTION__ );
      lua_pushnil( L );
      return;
   }

   box = (ACCOUNT_DATA **)lua_newuserdata( L, sizeof( ACCOUNT_DATA * ) );
   luaL_getmetatable( L, "Account.meta" );
   if( lua_isnil( L, -1 ) )
   {
      bug( "%s: Account.meta is missing.", __FUNCTION__ );
      lua_pop( L, 2 );
      lua_pushnil( L );
      return;
   }
   lua_setmetatable( L, -2 );
   *box = account;
   return;
}
