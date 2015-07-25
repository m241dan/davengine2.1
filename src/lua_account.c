#include "mud.h"

const struct luaL_Reg AccountLib_m[] = {
   /* setters */
   { "setName", account_setName },
   { "setPassword", account_setPasswd },
   /* getters */
   { "getName", account_getName },
   { "getLevel", account_getLevel },
   /* utility */
   { "verifyPassword", account_verifyPasswd },
   { "msg", account_Message },
   { NULL, NULL }
};

const struct luaL_Reg AccountLib_f[] = {
   { "new", newAccount },
   { "get", getAccount },
   { "checkExists", checkExistsAccount },
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

/* account lib functions */

int newAccount( lua_State *L )
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
   lua_pushobj( L, account, ACCOUNT_DATA );
   return 1;
}

int getAccount( lua_State *L )
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
            lua_pushobj( L, account, ACCOUNT_DATA );
         break;
      case LUA_TNUMBER:
         if( ( account = get_accountByID( lua_tonumber( L, -1 ) ) ) == NULL )
            lua_pushnil( L );
         else
            lua_pushobj( L, account, ACCOUNT_DATA );
         break;
   }
   return 1;
}

int checkExistsAccount( lua_State *L )
{
   MYSQL_ROW row;
   char query[MAX_BUFFER];

   switch( lua_type( L, -1 ) )
   {
      default:
         bug( "%s: bad argument passed.", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      case LUA_TSTRING:
         snprintf( query, MAX_BUFFER, "SELECT accountID FROM `accounts` WHERE name='%s' LIMIT 1;", lua_tostring( L, -1 ) );
         if( ( row = db_query_single_row( query ) ) != NULL )
         {
            lua_pushnumber( L, atoi( row[0] ) );
            break;
         }
         lua_pushnil( L );
         break;
      case LUA_TNUMBER:
         snprintf( query, MAX_BUFFER, "SELECT name FROM `accounts` WHERE accountID=%d LIMIT 1;", (int)lua_tonumber( L, -1 ) );
         if( ( row = db_query_single_row( query ) ) != NULL )
         {
            lua_pushstring( L, row[0] );
            break;
         }
         lua_pushnil( L );
         break;
   }
   FREE( row );
   return 1;
}

int delAccount( lua_State *L )
{
   ACCOUNT_DATA *account;

   DAVLUACM_ACCOUNT_NONE( account, L );
   if( lua_type( L, 2 ) != LUA_TSTRING )
   {
      bug( "%s: need to pass a password to delete an account" );
      return 0;
   }
   if( !verify_account_password( lua_tostring( L, 2 ), account ) )
   {
      bug( "%s: bad password given, did not delete.", __FUNCTION__ );
      return 0;
   }
   delete_account( account );
   return 0;
}

/* account object methods */

/* lua setters */

int account_setName( lua_State *L )
{
   ACCOUNT_DATA *account;

   DAVLUACM_ACCOUNT_BOOL( account, L );

   if( lua_gettop( L ) != 2 )
   {
      bug( "%s: incorrect number of parameters passed.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( lua_type( L, 1 ) != LUA_TSTRING || lua_type( L, 2 ) != LUA_TSTRING )
   {
      bug( "%s: argument 1 or arugment 2 are not of type string.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( !verify_account_password( lua_tostring( L, 2 ), account ) )
   {
      bug( "%s: bad password, could not change name.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   if( !check_name( lua_tostring( L, 1 ) ) )
   {
      bug( "%s: not a legal name, could not change.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   set_aName( account, lua_tostring( L, 1 ) );
   lua_pushboolean( L, 1 );
   return 1;
}

int account_setPasswd( lua_State *L )
{
   ACCOUNT_DATA *account;

   DAVLUACM_ACCOUNT_BOOL( account, L );

   if( lua_type( L, 1 ) != LUA_TSTRING || lua_type( L, 2 ) != LUA_TSTRING )
   {
      bug( "%s: argument 1 or argument 2 are not of type string.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( !set_aPasswd( account, lua_tostring( L, 1 ), lua_tostring( L, 2 ) ) )
   {
      bug( "%s: invalid oldpw passed.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   lua_pushboolean( L, 1 );
   return 1;
}

/* lua getters */

int account_getName( lua_State *L )
{
   ACCOUNT_DATA *account;

   DAVLUACM_ACCOUNT_NIL( account, L );
   lua_pushstring( L, get_aName( account ) );
   return 1;
}
int account_getLevel( lua_State *L )
{
   ACCOUNT_DATA *account;

   DAVLUACM_ACCOUNT_NIL( account, L );
   lua_pushnumber( L, get_aLevel( account ) );
   return 1;
}


/* utility */

int account_verifyPasswd( lua_State *L )
{
   ACCOUNT_DATA *account;

   DAVLUACM_ACCOUNT_BOOL( account, L );
   if( lua_type( L, 2 ) != LUA_TSTRING )
   {
      bug( "%s: need to pass a string to verify if its the password...", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   if( !verify_account_password( lua_tostring( L, 2 ), account ) )
   {
      lua_pushboolean( L, 0 );
      return 1;
   }
   lua_pushboolean( L, 1 );
   return 1;
}

int account_Message( lua_State *L )
{
   ACCOUNT_DATA *account;
   D_SOCKET *socket;
   int top, buffer_id = 0;

   DAVLUACM_ACCOUNT_NONE( account, L );

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
   if( !account->managing_state || ( socket = account->managing_state->socket ) == NULL )
   {
      bug( "%s: cannot send message, account has no socket.", __FUNCTION__ );
      return 0;
   }

   __text_to_buffer( socket, lua_tostring( L, 2 ), buffer_id );
   return 0;
}

/* int account_addChar( lua_State *L ); */

