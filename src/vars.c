/* vars.c written by Davenge */

#include "mud.h"

const char *const settable_vars[] = {
   "Account.meta", "Nanny.meta",
   '\0'
};

const struct luaL_Reg VarLib_m[] = {
   /* setters */
   /* getters */
   /* utility */
   { NULL, NULL }
};

const struct luaL_Reg VarLib_f[] = {
   { "new", newVar },
   { "get", getVar },
   { "set", setVar },
   { "del", delVar },
   { NULL, NULL }
};

int luaopen_VarLib( lua_State *L )
{
   luaL_newmetatable( L, "Var.meta" );

   lua_pushvalue( L, -1 );
   lua_setfield( L, -2, "__index" );

   lua_pushcfunction( L, VarGC );
   lua_setfield( L, -2, "__gc" );

   lua_pushcfunction( L, newVarIndex );
   lua_setfield( L, -2, "__newindex" );

   luaL_setfuncs( L, VarLib_m, 0 );

   luaL_newlib( L, VarLib_f );
   return 1;
}

int VarGC( lua_State *L )
{
   LUA_VAR *var;

   var = *(LUA_VAR **)lua_touserdata( L, -1 );
   free_var( var );
   return 0;
}

int newVar( lua_State *L )
{
   LUA_VAR *var;
   LUA_INDEX index;
   LUA_DATA data;
   int ownertype, ownerid;
   int top = lua_gettop( L );

   if( top < 1 )
   {
      bug( "%s: bad number of args passed, need at least 1.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   if( lua_type( L, 1 ) != LUA_TSTRING || strlen( lua_tostring( L, 1 ) ) > 60 )
   {
      bug( "%s: var name must be of type string or string is longer than 60 chars", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   if( top > 1 )
   {
      switch( lua_type( L, 2 ) )
      {
         default:
            bug( "%s: lua vars can only store numbers or strings.", __FUNCTION__ );
            lua_pushnil( L );
            return 1;
         case LUA_TNUMBER:
            data.type = TAG_INT;
            snprintf( data.data, MAX_BUFFER, "%d", (int)lua_tonumber( L, 2 ) );
            break;
         case LUA_TSTRING:
            data.type = TAG_STRING;
            snprintf( data.data, MAX_BUFFER, "%s", lua_tostring( L, 2 ) );
            break;
         case LUA_TNIL:
            data.type = TAG_INT;
            snprintf( data.data, MAX_BUFFER, "%d", 0 );
            break;
      }
   }

   if( top > 2 )
   {
      if( lua_type( L, 3 ) != LUA_TUSERDATA )
      {
         bug( "%s: defaulting to global, cannot set var to non-userdata", __FUNCTION__ );
         ownertype = GLOBAL_TAG;
         ownerid = get_potential_id( GLOBAL_TAG );
      }
      else
      {
         int metatype;
         if( ( metatype = get_meta_type_id( L, 3 ) ) == -1 )
         {
            bug( "%s: unknown meta type, defaulting to global owner.", __FUNCTION__ );
            ownertype = GLOBAL_TAG;
            ownerid = get_potential_id( GLOBAL_TAG );
         }
         else
         {
            ownertype = metatype;
            ownerid = get_meta_id( L, 3 );
         }
      }
   }
   else
   {
      ownertype = GLOBAL_TAG;
      ownerid = get_potential_id( GLOBAL_TAG );
   }

   var = init_var();
   var->ownertype = ownertype;
   var->ownerid = ownerid;
   var->name = new_string( lua_tostring( L, 1 ) );

   if( check_exists( var ) )
   {
      bug( "%s: trying to create a var that already exists.", __FUNCTION__ );
      lua_pushnil( L );
      free_var( var );
      return 1;
   }
   /* finish ownership and indexing */
   if( var->ownertype == GLOBAL_TAG )
      var->ownerid = get_new_id( GLOBAL_TAG );
   standard_index( &index );

   /* default script */
   data.isscript = FALSE;
   memset( &data.script[0], 0, sizeof( data.script ) );
   /* update database */
   new_var( var, &index, &data );

   lua_pushobj( L, var, LUA_VAR );
   return 1;
}

int setVar( lua_State *L )
{
   LUA_VAR *var;
   int new_ownertype, new_ownerid, top = lua_gettop( L );

   if( top != 2 )
   {
      bug( "%s: improper number of arguments passed", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( lua_type( L, 1 ) != LUA_TUSERDATA || !check_meta( L, 1, "Var.meta" ) )
   {
      bug( "%s: arg 1 needs to be a Var.meta", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( ( lua_type( L, 2 ) != LUA_TUSERDATA && lua_type( L, 2 ) != LUA_TNIL ) || !varsettable( L, 2 ) )
   {
      bug( "%s: arg 2 needs to be a valid var owner", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   var = *(LUA_VAR **)lua_touserdata( L, 1 );
   if( lua_type( L, 2 ) != LUA_TNIL )
   {
      new_ownertype = get_meta_type_id( L, 2 );
      new_ownerid = get_meta_id( L, 2 );
   }
   else
   {
      new_ownertype = GLOBAL_TAG;
      new_ownerid = get_new_id( GLOBAL_TAG );
   }
   bug( "%s: newot %d newoi %d", __FUNCTION__, new_ownertype, new_ownerid );
   bug( "%s: oldot %d oldoi %d", __FUNCTION__, var->ownertype, var->ownerid );
   bug( "%s: varname %s length %d", __FUNCTION__, var->name, strlen( var->name ) );
   if( !quick_query( "UPDATE `vars` SET ownertype=%d, ownerid=%d WHERE ownertype=%d AND ownerid=%d and name='%s';",
      new_ownertype, new_ownerid, var->ownertype, var->ownerid, var->name ) )
   {
      bug( "%s: could not change the vars ownership in the database.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   var->ownertype = new_ownertype;
   var->ownerid = new_ownerid;
   lua_pushboolean( L, 1 );
   return 1;
}

int getVar( lua_State *L )
{
   LUA_VAR *var;
   TAG_TYPE type;

   if( lua_type( L, 1 ) != LUA_TSTRING )
   {
      bug( "%s: arg 1 expected name of var.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   if( lua_gettop( L ) == 2 ) /* should be user data */
   {
      if( lua_type( L, 2 ) != LUA_TUSERDATA )
      {
         bug( "%s: attempting to get var from non-user data.", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      }
      type = get_meta_type_id( L, 2 );
      if( type == -1 || type == 3 ) /* if its -1(not found) or 3(var.meta) */
      {
         bug( "%s: attempting to get var from invalid user data.", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      }
   }
   else /* otherwise default to global */
      type = GLOBAL_TAG;
   var = init_var();
   var->ownertype = type;
   if( type == GLOBAL_TAG )
      var->ownerid = get_global_var_id( lua_tostring( L, 1 ) );
   else
      var->ownerid = get_meta_id( L, 2 );
   var->name = new_string( lua_tostring( L, 1 ) );

   if( check_exists( var ) )
      lua_pushobj( L, var, LUA_VAR );
   else
   {
      free_var( var );
      lua_pushnil( L );
   }

   return 1;
}

int delVar( lua_State *L )
{
   return 0;
}

int newVarIndex( lua_State *L )
{
   return 0;
}

/* creation */
LUA_VAR *init_var( void )
{
   LUA_VAR *var;
   var 			= malloc( sizeof( LUA_VAR ) );
   var->tag		= init_tag();
   var->tag->type	= VAR_TAG;
   var->ownertype	= TAG_UNSET;
   var->ownerid		= 0;
   var->name		= NULL;
   return var;
}

void standard_index( LUA_INDEX *index )
{
   index->type = TAG_INT;
   snprintf( index->index, 60, "%d", 0 );
}

bool new_var( LUA_VAR *var, LUA_INDEX *index, LUA_DATA *data )
{
   if( !quick_query( "INSERT INTO `vars` VALUES( %d, %d, '%s', %d, '%s', %d, '%s', '%s', '%d' ) ON DUPLICATE KEY UPDATE datatype=%d, data='%s';",
      var->ownertype, var->ownerid, var->name, index->type, index->index, data->type, data->data, data->script, data->isscript, data->type, data->data ) )
   {
      return FALSE;
   }
   return TRUE;
}

/* deletion */
void free_var( LUA_VAR *var )
{
   free_tag( var->tag );
   var->tag = NULL;
   FREE( var->name );
   FREE( var );
}

void delete_var( LUA_VAR *var )
{
   if( var->ownertype == GLOBAL_TAG )
   {
      if( !quick_query( "INSERT INTO `id_recycled` VALUES( '%d', '%d' );", var->ownertype, var->ownerid ) )
         bug( "%s: did not update recycled ids database with tag %d of type %d.", __FUNCTION__, var->ownerid, var->ownertype );
   }
   if( !quick_query( "DELETE FROM `vars` WHERE ownertype=%d AND ownerid=%d AND name='%s';", var->ownertype, var->ownerid, var->name ) )
      bug( "%s: could not delete the var and its indexs", __FUNCTION__ );
   free_var( var );
}

void delete_index( LUA_VAR *var, LUA_INDEX *index )
{
   if( !quick_query( "DELETE FROM `vars` WHERE ownertype=%d AND ownerid=%d AND name='%s' AND indextype=%d AND index='%s';",
      var->ownertype, var->ownerid, var->name, index->type, index->index ) )
      bug( "%s: could not delete the index", __FUNCTION__ );
}

/* getters */
int get_global_var_id( const char *name )
{
   MYSQL_ROW row;
   char query[MAX_BUFFER];
   int id;

   snprintf( query, MAX_BUFFER, "SELECT ownerid FROM `vars` WHERE ownertype=%d AND name='%s';", GLOBAL_TAG, name );
   if( ( row = db_query_single_row( query ) ) == NULL )
      return 0;

   id = atoi( row[0] );
   FREE( row );
   return id;
}

/* utility */
bool check_exists( LUA_VAR *var )
{
   MYSQL_ROW row;
   char query[MAX_BUFFER];
   bool exists = FALSE;
   if( var->ownertype == GLOBAL_TAG )
      snprintf( query, MAX_BUFFER, "SELECT ownertype FROM `vars` WHERE ownertype=%d AND name='%s';",
               var->ownertype, var->name );
   else
      snprintf( query, MAX_BUFFER, "SELECT ownertype FROM `vars` WHERE ownertype=%d AND ownerid=%d AND name='%s';",
               var->ownertype, var->ownerid, var->name );
   if( ( row = db_query_single_row( query ) ) != NULL )
      exists = TRUE;

   FREE( row );
   return exists;
}

bool varsettable( lua_State *L, int index )
{
   if( lua_type( L, index ) == LUA_TNIL )
      return TRUE;
   int metatype = get_meta_type_id( L, index );
   for( int x = 0; settable_vars[x] != '\0'; x++ )
      if( !strcmp( settable_vars[x], meta_types[metatype] ) )
         return TRUE;
   return FALSE;
}
