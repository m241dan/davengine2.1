/* Davengine Lua C API utilities written by Davenge */

#include "mud.h"

const char *const meta_types[MAX_TAG_TYPE+1] = {
   "Account.meta", "Nanny.meta", '\0'
};

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

   log_string( "Loading Account Lua Lib" );
   luaL_requiref( lua_handle, "Account", luaopen_AccountLib, 1 );
   lua_pop( lua_handle, -1 );

   log_string( "Loading Nanny Lua Lib" );
   luaL_requiref( lua_handle, "Nanny", luaopen_NannyLib, 1 );
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


bool prep_stack_handle( lua_State *handle, const char *file, const char *function )
{
   int ret;

   if( !handle )
   {
      bug( "%s: the lua stack isn't initialized", __FUNCTION__ );
      return FALSE;
   }

   if( !file || !function )
   {
      bug( "%s: the file or function passed is NULL.", __FUNCTION__ );
      return FALSE;
   }

   lua_pushnil( handle );
   lua_setglobal( handle, function );

   if( ( ret = luaL_loadfile( handle, file ) ) != 0 )
   {
      if( ret != LUA_ERRFILE )
         bug( "%s: %s: %s\n\r", __FUNCTION__, function, lua_tostring( handle, -1 ) );
      lua_pop( handle, 1 );
      return FALSE;
   }

   if( ( ret = lua_pcall( handle, 0, 0, 0 ) ) != 0 )
   {
      bug( "%s: ret %d: path: %s\r\n - error message: %s.", __FUNCTION__, ret, file, lua_tostring( handle, -1 ) );
      lua_pop( handle, 1 );
      return FALSE;
   }

   lua_getglobal( handle, function );
   if( lua_isnil( handle, -1 ) )
   {
      lua_pop( handle, -1 );
      return FALSE;
   }
   return TRUE;
}

void *check_meta( lua_State *L, int index, const char *meta_name )
{
   void *object = lua_touserdata( L, index );

   if( !object )
      return NULL;

   if( lua_getmetatable( L, index ) )
   {
      luaL_getmetatable( L, meta_name );
      if( !lua_rawequal( L, -1, -2 ) )
         object = NULL;
      lua_pop( L, 2 );
      return object;
   }
   else
      return NULL;
}
