/* vars.c written by Davenge */

#include "mud.h"

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
   LUA_INDEX *index;
   LUA_DATA *data;
   int datatype, ownertype, ownerid;
   long int datadata;
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
            datatype = TAG_INT;
            datadata = lua_tonumber( L, 2 );
            break;
         case LUA_TSTRING:
            datatype = TAG_STRING;
            datadata = (long int)new_string( lua_tostring( L, 2 ) );
            break;
         case LUA_TNIL:
            datatype = TAG_INT;
            datadata = 0;
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
            ownerid = get_meta_id( L, 3, meta_types[metatype] );
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
   var->ownerid = get_new_id( GLOBAL_TAG );
   index = standard_index( var );
   data = init_vardata( var );
   data->type = datatype;
   data->data = datadata;
   new_var( var, index, data );
   free_varindex( index );
   free_vardata( data );
   lua_pushobj( L, var, LUA_VAR );
   return 1;
}

int setVar( lua_State *L )
{
   return 0;
}

int getVar( lua_State *L )
{
   return 0;
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

LUA_INDEX *init_varindex( LUA_VAR *var )
{
   LUA_INDEX *index;
   index	= malloc( sizeof( LUA_INDEX ) );
   index->owner	= var;
   index->type	= TAG_UNSET;
   index->index	= 0;
   return index;
}

LUA_DATA *init_vardata( LUA_VAR *var )
{
   LUA_DATA *data;
   data		= malloc( sizeof( LUA_DATA ) );
   data->owner	= var;
   data->type	= TAG_UNSET;
   data->data	= 0;
   return data;
}

LUA_INDEX *standard_index( LUA_VAR *var )
{
   LUA_INDEX *index = init_varindex( var );
   index->type = TAG_INT;
   index->index = 0;
   return index;
}

bool new_var( LUA_VAR *var, LUA_INDEX *index, LUA_DATA *data )
{
   char index_query[60];
   char *data_query;

   if( index->type == TAG_STRING )
      snprintf( index_query, 60, "%s", (char *)index->index );
   else
      snprintf( index_query, 60, "%ld", index->index );

   if( data->type == TAG_STRING )
      data_query = new_string( (char *)data->data );
   else
      data_query = new_string( "%ld", data->data );

   if( !quick_query( "INSERT INTO `vars` VALUES( %d, %d, '%s', %d, '%s', %d, '%s' ) ON DUPLICATE KEY UPDATE datatype=%d, data='%s';",
      var->ownertype, var->ownerid, var->name, index->type, index_query, data->type, data_query, data->type, data_query ) )
   {
      FREE( data_query );
      return FALSE;
   }
   FREE( data_query );
   return TRUE;
}

/* deletion */
void free_var( LUA_VAR *var )
{
   free_tag( var->tag );
   if( var->ownertype == GLOBAL_TAG )
   {
      if( !quick_query( "INSERT INTO `id_recycled` VALUES( '%d', '%d' );", var->ownertype, var->ownerid ) )
         bug( "%s: did not update recycled ids database with tag %d of type %d.", __FUNCTION__, var->ownerid, var->ownertype );
   }
   FREE( var->name );
   FREE( var );
}

void free_varindex( LUA_INDEX *index )
{
   char *string;
   index->owner = NULL;
   if( index->type == TAG_STRING )
   {
      string = (char *)index->index;
      FREE( string );
   }
   FREE( index );
}

void free_vardata( LUA_DATA *data )
{
   char *string;
   data->owner = NULL;
   if( data->type == TAG_STRING )
   {
      string = (char *)data->data;
      FREE( string );
   }
   FREE( data );
}

/* getters */

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
