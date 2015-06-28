/* vars.c written by Davenge */

#include "mud.h"

const struct luaL_Reg VarLib_m[] = {
   /* setters */
   /* getters */
   /* utility */
   { NULL, NULL }
};

const struct luaL_Reg VarLib_f[] = {
   { NULL, NULL }
};

int luaopen_VarLib( lua_State *L )
{
   luaL_newmetatable( L, "Var.meta" );

   lua_pushvalue( L, -1 );
   lua_setfield( L, -2, "__index" );

   lua_pushcfunction( L, VarGC );
   lua_setfield( L, -2, "__gc" );

   luaL_setfuncs( L, VarLib_m, 0 );

   luaL_newlib( L, VarLib_f );
   return 1;
}

int VarGC( lua_State *L )
{
   DetachFromList( 
   return 0;
}

int newVar( lua_State *L )
{
   LUA_VAR *var;
   LUA_INDEX *index;
   LUA_DATA *data;
   char ref[MAX_BUFFER];
   int indextype, datatype, ownertype, ownerid;
   long int indexindex, datadata;
   int top = lua_gettop( L );
   if( top < 1 )
   {
      bug( "%s: bad number of args passed, need at least 1.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   if( lua_var( L, 1 ) != LUA_TSTRING || strlen( lua_tostring( L, 1 ) ) > 60 )
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
            datadata = new_string( lua_tostring( L, 2 ) );
            break;
         case LUA_TNIL:
            datatype = TAG_INT;
            datadata = 0;
            break;
      }
   }

   if( top( L ) > 2 )
   {
      if( lua_type( L, 3 ) != LUA_TUSERDATA )
      {
         bug( "%s: defaulting to global, cannot set var to non-userdata", __FUNCTION__ );
         ownertype = GLOBAL_TAG;
         ownerid = get_new_id( GLOBAL_TAG );
      }
      else
      {
         int metatype;
         if( ( metatype = get_meta_type_id( L, 3 ) ) == -1 )
         {
            bug( "%s: unknown meta type, defaulting to global owner.", __FUNCTION__ );
            ownertype = GLOBAL_TAG;
            ownerid = get_new_id( GLOBAL_TAG );
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
      ownerid = get_new_id( GLOBAL_TAG );
   }

   if( ( var = get_var_from_monitor( ownertype, ownerid, lua_tostring( L, 1 ) ) ) == NULL )
   {
      var = (LUA_VAR *)lua_newuserdata( L, sizeof( LUA_VAR ) );
      var->ownertype = ownertype;
      var->ownerid = ownerid;
      var->name = new_string( lua_tostring( L, 1 ) );
      luaL_getmetatable( L, "Var.meta" );
      lua_setmetatable( L, -2 );
      snprintf( ref, MAX_BUFFER, "%d", (int)&var );
      lua_setfield( L, LUA_REGISTRYINDEX, ref );
   }
   else
   {
      snprintf( ref, MAX_BUFFER, "%d", (int)&var );
      lua_getfield( L, LUA_REGISTRYINDEX, ref );
   }
   return 1;
}

int setVar( lua_State *L )
{

}

int getVar( lua_State *L )
{

}

int delVar( lua_State *L )
{

}

/* creation */
LUA_VAR *init_var( void )
{
   LUA_VAR *var;
   var 			= malloc( sizeof( LUA_VAR ) );
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
   LUA_INDEX *index = init_varindex( var ) );
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
      snprintf( index_query, 60, "%d", index->index );

   if( data->type == TAG_STRING )
      data_query = new_string( (char *)data->data );
   else
      data_query = new_string( "%d", data->data );

   if( !quick_query( "INSERT INTO `vars` VALUES( %d, %d, '%s', %d, '%s', %d, '%s' ) ON DUPLICATE KEY UPDATE datatype=%d, data='%s';", ) )
   {
      FREE( data_query );
      return FALSE;
   }
   FREE( data_query );
   return TRUE;
}

/* deletion */
bool free_var( LUA_VAR *var )
{
   if( ListHas( lua_Vars, var ) )
   {
      bug( "%s: cannot free, still attached to global monitor list", __FUNCTION__ );
      return FALSE;
   } 
   FREE( var->name );
   FREE( var );
   return TRUE;
}

void free_varindex( LUA_INDEX *index )
{
   index->owner = NULL;
   if( index->type == TAG_STRING )
      FREE( index->index );
   FREE( index );
}

void free_vardata( LUA_DATA *data )
{
   data->owner = NULL;
   if( data->type == TAG_STRING )
      FREE( data->data );
   FREE( data );
}

/* getters */
LUA_VAR *get_var_from_monitor( TAG_TYPE ownertype, int id, const char *name )
{
   LUA_VAR *var;
   ITERATOR Iter;

   AttachIterator( &Iter, lua_vars );
   while( ( var = (LUA_VAR *)NextInList( &Iter ) ) != NULL )
      if( var->ownertype == ownertype && var->ownerid == id && !strcmp( var->name, name ) )
         break;
   DetachIterator( &Iter );

   return var;
}


/* utility */
bool check_exists( LUA_VAR *var, LUA_INDEX *index )
{
   MYSQL_ROW row;
   char query[MAX_BUFFER];
   bool exists = FALSE;

   snprintf( query, MAX_BUFFER, "SELECT ownertype FROM `vars` WHERE ownertype=%d AND ownerid=%d AND name='%s' AND indextype=%d AND index='%s';",
            var->ownertype, var->ownerid, var->name, index->type, index->index );
   if( ( row = db_query_single_row( query ) ) != NULL )
      exists = TRUE;

   FREE( row );
   return exists;
}

void lua_pushvar( lua_State *L, LUA_VAR *var )
{
   if( !var )
   {
       bug( "%s: trying to push a NULL instance.", __FUNCTION__ );
       lua_pushnil( L );
       return;
   }
   box = (ENTITY_INSTANCE **)lua_newuserdata( L, sizeof( ENTITY_INSTANCE * ) );
   
   luaL_getmetatable( L, "EntityInstance.meta" );
   if( lua_isnil( L, -1 ) )
   {
      bug( "%s: EntityInstance.meta is missing.", __FUNCTION__ );
      lua_pop( L, -1 ); /* pop meta */
      lua_pop( L, -1 ); /* pop box */
      lua_pushnil( L );
      return;
   }
   lua_setmetatable( L, -2 );

   *box = instance;
   return;

}
