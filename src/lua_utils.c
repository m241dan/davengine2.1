/* Davengine Lua C API utilities written by Davenge */

#include "mud.h"

const char *const meta_types[MAX_TAG_TYPE+1] = {
   "Account.meta", "Nanny.meta", "Global.meta", "Var.meta", "Socket.meta", "Entity.meta",
   '\0'
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

   log_string( "Loading Var Lua Lib" );
   luaL_requiref( lua_handle, "var", luaopen_VarLib, 1 );
   lua_pop( lua_handle, -1 );

   log_string( "Loading Account Lua Lib" );
   luaL_requiref( lua_handle, "Account", luaopen_AccountLib, 1 );
   lua_pop( lua_handle, -1 );

   log_string( "Loading Nanny Lua Lib" );
   luaL_requiref( lua_handle, "Nanny", luaopen_NannyLib, 1 );
   lua_pop( lua_handle, -1 );

   log_string( "Loading Socket Lua Lib" );
   luaL_requiref( lua_handle, "Socket", luaopen_SocketLib, 1 );
   lua_pop( lua_handle, -1 );

   log_string( "Loading Entity Lua Lib" );
   luaL_requiref( lua_handle, "Entity", luaopen_EntityLib, 1 );
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

int get_meta_type_id( lua_State *L, int index )
{
  int x, metatype = -1;

   for( x = 0; meta_types[x] != '\0'; x++ )
      if( check_meta( L, index, meta_types[x] ) )
         metatype = x;
   return metatype;
}

int get_meta_id( lua_State *L, int index )
{
   if( lua_type( L, index ) != LUA_TUSERDATA )
   {
      bug( "%s: data at index must be of type userdata to get an id.", __FUNCTION__ );
      return -1;
   }

   switch( get_meta_type_id( L, index ) )
   {
      default:
         bug( "%s: unknown metatype for searching out meta data IDs.", __FUNCTION__ );
         return -1;
      case ACCOUNT_TAG:
      {
         ACCOUNT_DATA *account = *(ACCOUNT_DATA **)lua_touserdata( L, index );
         return GET_ID( account );
      }
      case NANNY_TAG:
      {
         NANNY_DATA *nanny = *(NANNY_DATA **)lua_touserdata( L, index );
         return GET_ID( nanny );
      }
      case GLOBAL_TAG:
      case VAR_TAG:
      {
         LUA_VAR *var = *(LUA_VAR **)lua_touserdata( L, index );
         return var->ownerid;
      }
      case ENTITY_TAG:
      {
         ENTITY_DATA *entity = *(ENTITY_DATA **)lua_touserdata( L, index );
         return GET_ID( entity );
      }
   }
}

void lua_pushstateobj( lua_State *L, SOCKET_STATE *state )
{
   switch( state->type )
   {
      default: return;
      case NANNY_TAG:
      {
         NANNY_DATA *nanny;
         if( ( nanny = state->control.nanny ) == NULL )
         {
            bug( "%s: nanny state has no nanny assigned to it.", __FUNCTION__ );
            return;
         }
         lua_pushobj( L, nanny, NANNY_DATA );
         break;
      }
      case ACCOUNT_TAG:
      {
         ACCOUNT_DATA *account;
         if( ( account = state->control.account ) == NULL )
         {
            bug( "%s: account state has no account assigned to it.", __FUNCTION__ );
            return;
         }
         lua_pushobj( L, account, ACCOUNT_DATA );
         break;
      }
      case ENTITY_TAG:
      {
         ENTITY_DATA *entity;
         if( ( entity = state->control.entity ) == NULL )
         {
            bug( "%s: entity state has no entity assigned to it.", __FUNCTION__ );
            return;
         }
         lua_pushobj( L, entity, ENTITY_DATA );
         break;
      }
   }
}

void lua_onConnect( D_SOCKET *socket )
{
   int ret, top = lua_gettop( lua_handle );
   prep_stack( "scripts/events/globals/onConnect.lua", "onConnect" );
   lua_pushobj( lua_handle, socket, D_SOCKET );
   if( ( ret = lua_pcall( lua_handle, 1, LUA_MULTRET, 0 ) ) )
      bug( "%s: ret %d: path %s\r\n - error message: %s.", __FUNCTION__, ret, "scripts/events/globals/onConnect.lua", lua_tostring( lua_handle, -1 ) );
   lua_settop( lua_handle, top );
}
