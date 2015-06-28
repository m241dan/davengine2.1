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
   return 0;
}
( "name", "value", owner )
int newVar( lua_State *L )
{
   LUA_VAR *var;
   LUA_INDEX *var;
   LUA_DATA *var;
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
            
         case LUA_TSTRING:
         case LUA_TNIL:
         
      }
   }

   if( top( L ) > 2 )


}

int setVar( lua_State *L )
{

}

int getVar( lua_State *L )
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

void    free_varindex   ( LUA_INDEX *index );
void    free_vardata    ( LUA_DATA *data );

