/* vars.c written by Davenge */

#include "mud.h"

const char *const settable_vars[] = {
   "Account.meta", "State.meta", "Entity.meta", "Socket.meta",
   '\0'
};

const struct luaL_Reg VarLib_f[] = {
   { "new", newVar },
   { "get", getVar },
   { "set", setVar },
   { "del", delVar },
   { "getOwner", getVarOwner },
   { "setScript", setScript },
   { "iterate", iterateVar },
   { NULL, NULL }
};

int luaopen_VarLib( lua_State *L )
{
   luaL_newmetatable( L, "Var.meta" );

   lua_pushcfunction( L, getVarIndex );
   lua_setfield( L, -2, "__index" );

   lua_pushcfunction( L, VarGC );
   lua_setfield( L, -2, "__gc" );

   lua_pushcfunction( L, newVarIndex );
   lua_setfield( L, -2, "__newindex" );

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

inline int newVar( lua_State *L )
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

   if( lua_type( L, 1 ) != LUA_TSTRING || strlen( lua_tostring( L, 1 ) ) > MAX_VINDEX )
   {
      bug( "%s: var name must be of type string or string is longer than %d chars", __FUNCTION__, MAX_VINDEX );
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
   /* update database */
   new_var( var, &index, &data );

   lua_pushobj( L, var, LUA_VAR );
   return 1;
}

inline int setVar( lua_State *L )
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

inline int getVar( lua_State *L )
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

inline int delVar( lua_State *L )
{
   LUA_VAR *var;

   if( lua_type( L, 1 ) != LUA_TUSERDATA || !check_meta( L, 1, "Var.meta" ) )
   {
      bug( "%s: non-Var.meta passed as arg.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   var = *(LUA_VAR **)lua_touserdata( L, 1 );
   delete_var_from_db( var );
   lua_pushboolean( L, 1 );
   return 1;
}

inline int newVarIndex( lua_State *L )
{
   LUA_VAR *var;
   LUA_INDEX index;
   LUA_DATA data;

   DAVLUACM_VAR_NONE( var, L );

   switch( lua_type( L, 2 ) )
   {
      default:
         bug( "%s: invalid Var.meta index used.", __FUNCTION__ );
         return 0;
      case LUA_TSTRING:
         index.type = TAG_STRING;
         snprintf( index.index, MAX_VINDEX, "%s", lua_tostring( L, 2 ) );
         break;
      case LUA_TNUMBER:
         index.type = TAG_INT;
         snprintf( index.index, MAX_VINDEX, "%d", (int)lua_tonumber( L, 2 ) );
         break;
   }

   switch( lua_type( L, 3 ) )
   {
      default:
         bug( "%s: invalid Var.meta data used.", __FUNCTION__ );
         return 0;
      case LUA_TSTRING:
         data.type = TAG_STRING;
         snprintf( data.data, MAX_BUFFER, "%s", lua_tostring( L, 3 ) );
         break;
      case LUA_TNUMBER:
         data.type = TAG_INT;
         snprintf( data.data, MAX_BUFFER, "%d", (int)lua_tonumber( L, 3 ) );
         break;
      case LUA_TNIL:
         delete_index_from_db( var, &index );
         return 0;
   }
   data.isscript = FALSE;
   new_var( var, &index, &data );
   return 0;
}

inline int getVarIndex( lua_State *L )
{
   LUA_VAR *var;
   MYSQL_ROW row;
   LUA_INDEX index;
   LUA_DATA data;
   char query[MAX_BUFFER];

   /* we know that L,1 has to be something with Var.meta, so no need to sanity check */
   var = *(LUA_VAR **)lua_touserdata( L, 1 );

   switch( lua_type( L, 2 ) )
   {
      default:
         bug( "%s: invalid Var.meta index used.", __FUNCTION__ );
         return 0;
      case LUA_TSTRING:
         index.type = TAG_STRING;
         snprintf( index.index, MAX_VINDEX, "%s", lua_tostring( L, 2 ) );
         break;
      case LUA_TNUMBER:
         index.type = TAG_INT;
         snprintf( index.index, MAX_VINDEX, "%d", (int)lua_tonumber( L, 2 ) );
         break;
   }

   snprintf( query, MAX_BUFFER, "SELECT datatype, data, isscript FROM `vars` WHERE ownertype=%d AND ownerid=%d AND name='%s' AND indextype=%d AND vindex='%s'",
            var->ownertype, var->ownerid, var->name, index.type, index.index );

   if( ( row = db_query_single_row( query ) ) == NULL )
   {
      lua_pushboolean( L, 0 );
      return 1;
   }
   db_load_data( &data, &row );
   if( data.isscript )
   {
      int ret, top = lua_gettop( L );
      if( !prep_stack_handle( L, data.data, index.index ) )
      {
         bug( "%s: could not prep the stack for file %s and function %s.", __FUNCTION__, data.data, index.index );
         lua_pushnil( L );
         return 1;
      }
      lua_pushvalue( L, 1 );
      if( ( ret = lua_pcall( L, 1, LUA_MULTRET, 0 ) ) )
      {
         bug( "%s: red %d: path %s\r\n - error message: %s.", __FUNCTION__, ret, data.data, lua_tostring( lua_handle, -1 ) );
         lua_pushnil( L );
         return 1;
      }
      int new_top = lua_gettop( L ) - top;
      return new_top < 0 ? 0 : new_top;
   }
   switch( data.type )
   {
      default:
         bug( "%s: bad data type, pushing nil.", __FUNCTION__ );
         lua_pushnil( L );
         break;
      case TAG_STRING:
         lua_pushstring( L, data.data );
         break;
      case TAG_INT:
         lua_pushnumber( L, atoi( data.data ) );
         break;
   }
   return 1;
}

inline int getVarOwner( lua_State *L )
{
   LUA_VAR *var;

   if( ( var = *(LUA_VAR **)luaL_checkudata( L, -1, "Var.meta" ) ) == NULL )
   {
      bug( "%s: non-Var.meta passed as argument 1 of parameters.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }
   if( lua_pushvarowner( L, var ) )
      return 1;
   lua_pushnil( L );
   return 1;
}

inline int setScript( lua_State *L )
{
   LUA_VAR *var;
   int top = lua_gettop( L );

   if( top != 2 )
   {
      bug( "%s: improper number of arguments passed. Expected 2, got %d.", __FUNCTION__, top );
      lua_pushboolean( L, 0 );
      return 1;
   }
   /* example var.setScript( variable, "index" ) */
   DAVLUACM_VAR_BOOL( var, L );
   if( lua_type( L, 2 ) != LUA_TSTRING )
   {
      bug( "%s: argument 2 is not a string.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( !quick_query( "UPDATE `vars` SET isscript=1 WHERE ownertype=%d AND ownerid=%d AND name='%s' AND indextype=%d AND vindex='%s';",
      var->ownertype, var->ownerid, var->name, TAG_STRING, lua_tostring( L, 2 ) ) )
   {
      bug( "%s: could not turn var %s's index %s into a script.", __FUNCTION__, var->name, lua_tostring( L, 2 ) );
      lua_pushboolean( L, 0 );
      return 1;
   }
   lua_pushboolean( L, 1 );
   return 1;
}

inline int iterateVar( lua_State *L )
{
   MYSQL_RES *result, **box;
   LUA_VAR *var;
   char query[MAX_BUFFER];
   int size;

   DAVLUACM_VAR_NIL( var, L );

   snprintf( query, MAX_BUFFER, "SELECT indextype, vindex, datatype, data, isscript FROM `vars` WHERE ownertype=%d AND ownerid=%d AND name='%s';",
      var->ownertype, var->ownerid, var->name );

   if( !quick_query( query ) )
   {
      lua_pushnil( L );
      return 1;
   }

   if( ( result = mysql_store_result( sql_handle ) ) == NULL )
   {
      lua_pushnil( L );
      return 1;
   }

   if( ( size = mysql_num_fields( result ) ) == 0 )
   {
      lua_pushnil( L );
      mysql_free_result( result );
      return 1;
   }

   box = (MYSQL_RES **)lua_newuserdata( L, sizeof( MYSQL_RES * ) );
   luaL_getmetatable( L, "SqlRes.meta" );
   if( lua_isnil( L, -1 ) )
   {
      bug( "%s: SqlRes.meta is missing.", __FUNCTION__ );
      lua_pop( L, 2 );
      lua_pushnil( L );
      return 1;
   }
   lua_setmetatable( L, -2 );
   *box = result;

   lua_pushcclosure( L, iterate_var, 1 );
   return 1;
}

inline int iterate_var( lua_State *L )
{
   MYSQL_RES *result = *(MYSQL_RES **)lua_touserdata( L, lua_upvalueindex(1) );
   MYSQL_ROW row;

   if( ( row = mysql_fetch_row( result ) ) == NULL )
      return 0;

   switch( atoi( row[0] ) )
   {
      default:
         bug( "%s: bad index type, canceling iteration.", __FUNCTION__ );
         return 0;
      case TAG_STRING:
         lua_pushstring( L, row[1] );
         break;
      case TAG_INT:
         lua_pushnumber( L, atoi( row[1] ) );
         break;
   }

   switch( atoi( row[2] ) )
   {
      default:
         bug( "%s: bad data type, canceling iteration.", __FUNCTION__ );
         return 0;
      case TAG_STRING:
         lua_pushstring( L, row[3] );
         break;
      case TAG_INT:
         lua_pushnumber( L, atoi( row[3] ) );
         break;
   }
   return 2;
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
   snprintf( index->index, MAX_VINDEX, "%d", 0 );
}

bool new_var( LUA_VAR *var, LUA_INDEX *index, LUA_DATA *data )
{
   if( !quick_query( "INSERT INTO `vars` VALUES( %d, %d, '%s', %d, '%s', %d, '%s', '%d' ) ON DUPLICATE KEY UPDATE datatype=%d, data='%s';",
      var->ownertype, var->ownerid, var->name, index->type, index->index, data->type, data->data, (int)data->isscript, data->type, data->data ) )
   {
      return FALSE;
   }
   return TRUE;
}

int db_load_data( LUA_DATA *data, MYSQL_ROW *row )
{
   int counter = 0;
   data->type = atoi( (*row)[counter++] );
   snprintf( data->data, MAX_BUFFER, "%s", (*row)[counter++] );
   data->isscript = (bool)atoi( (*row)[counter++] );
   return counter;
}

/* deletion */
void free_var( LUA_VAR *var )
{
   free_tag( var->tag );
   var->tag = NULL;
   FREE( var->name );
   FREE( var );
}

void delete_var_from_db( LUA_VAR *var )
{
   if( var->ownertype == GLOBAL_TAG )
   {
      if( !quick_query( "INSERT INTO `id_recycled` VALUES( '%d', '%d' );", var->ownertype, var->ownerid ) )
         bug( "%s: did not update recycled ids database with tag %d of type %d.", __FUNCTION__, var->ownerid, var->ownertype );
   }
   if( !quick_query( "DELETE FROM `vars` WHERE ownertype=%d AND ownerid=%d AND name='%s';", var->ownertype, var->ownerid, var->name ) )
      bug( "%s: could not delete the var and its indexs", __FUNCTION__ );
}

void delete_index_from_db( LUA_VAR *var, LUA_INDEX *index )
{
   if( !quick_query( "DELETE FROM `vars` WHERE ownertype=%d AND ownerid=%d AND name='%s' AND indextype=%d AND vindex='%s';",
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

bool lua_pushvarowner( lua_State *L, LUA_VAR *var )
{
   switch( var->ownertype )
   {
      default: return FALSE;
      case ACCOUNT_TAG: /* account */
      {
         ACCOUNT_DATA *account;
         if( ( account = get_accountByID_ifActive( var->ownerid ) ) == NULL )
            return FALSE;
         lua_pushobj( L, account, ACCOUNT_DATA );
         break;
      }
      case STATE_TAG: /* nanny */
      {
         SOCKET_STATE *state;
         if( ( nanny = get_nannyByID_ifActive( var->ownerid ) ) == NULL )
            return FALSE;
         lua_pushobj( L, state, SOCKET_STATE );
         break;
      }
      case GLOBAL_TAG: /* global */
         lua_pushstring( L, "global" );
         break;
      case ENTITY_TAG:
      {
         ENTITY_DATA *entity;
         if( ( entity = get_entityByID_ifActive( var->ownerid ) ) == NULL )
            return FALSE;
         lua_pushobj( L, entity, ENTITY_DATA );
      }
   }
   return TRUE;
}
