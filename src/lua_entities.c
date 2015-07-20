#include "mud.h"

const struct luaL_Reg EntityLib_m[] = {
   { NULL, NULL }
};

const struct luaL_Reg EntityLib_f[] = {
   { NULL, NULL }
};

int luaopen_EntityLib( lua_State *L )
{
   luaL_newmetatable( L, "Entity.meta" );

   lua_pushvalue( L, -1 );
   lua_setfield( L, -2, "__index" );

   lua_pushcfunction( L, EntityGC );
   lua_setfield( L, -2, "__gc" );

   luaL_setfuncs( L, EntityLib_m, 0 );

   luaL_newlib( L, EntityLib_f );
   return 1;
}

int EntityGC( lua_State *L )
{
   ENTITY_DATA **entity;
   entity = (ENTITY_DATA **)lua_touserdata( L, -1 );
   *entity = NULL;
   return 0;
}

/* entity lib functions */
int newEntity( lua_State *L )
{
   ENTITY_DATA *entity;

   if( lua_gettop( L ) != 1 )
   {
      bug( "%s: invalid number of arguments passed, one only please.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   if( lua_type( L, 1 ) != LUA_TSTRING )
   {
      bug( "%s: arg 1 invalid, should be of type string.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   if( !file_exists( lua_tostring( L, 1 ) ) )
   {
      bug( "%s: not a valid path to script file passed.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   entity = init_entity();
   entity_setScript( entity, lua_tostring( L, 1 ) );
   new_entity( entity );

   lua_pushobj( L, entity, ENTITY_DATA );
   return 1;
}

int getEntity( lua_State *L )
{
   ENTITY_DATA *entity;

   if( lua_gettop( L ) != 1 )
   {
      bug( "%s: invalid number of arguments passed, one only please.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   switch( lua_type( L, 1 ) )
   {
      default:
         bug( "%s: argument 1 of invalid type passed.", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      case LUA_TSTRING:
         bug( "%s: not ready for strings yet...", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      case LUA_TNUMBER:
         if( ( entity = get_entityByID( lua_tonumber( L, 1 ) ) ) != NULL )
            break;
         bug( "%s: could not find entity #%d.", __FUNCTION__, lua_tonumber( L, 1 ) );
         lua_pushnil( L );
         return 1;
   }

   lua_pushobj( L, entity, ENTITY_DATA );
   return 1;
}

int getEntity_ifActive( lua_State *L )
{
   ENTITY_DATA *entity;

   if( lua_gettop( L ) != 1 )
   {
      bug( "%s: invalid number of arguments passed, one only please.", __FUNCTION__ );
      lua_pushnil( L );
      return 1;
   }

   switch( lua_type( L, 1 ) )
   {
      default:
         bug( "%s: argument 1 of invalid type passed.", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      case LUA_TSTRING:
         bug( "%s: not ready for strings yet...", __FUNCTION__ );
         lua_pushnil( L );
         return 1;
      case LUA_TNUMBER:
         if( ( entity = get_entityByID_ifActive( lua_tonumber( L, 1 ) ) ) != NULL )
            break;
         bug( "%s: entity #%d does not appear to be active.", __FUNCTION__, lua_tonumber( L, 1 ) );
         lua_pushnil( L );
         return 1;
   }

   lua_pushobj( L, entity, ENTITY_DATA );
   return 1;
}

int delEntity( lua_State *L )
{
   ENTITY_DATA *entity;

   if( lua_gettop( L ) != 1 )
   {
      bug( "%s: invalid number of arguments passed, 1 only please.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   if( ( entity = *(ENTITY_DATA **)luaL_checkudata( L, 1, "Entity.meta" ) ) == NULL )
   {
      bug( "%s: bad userdata passed, should be of type Entity.meta", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   delete_entity( entity );
   lua_pushboolean( L, 1 );
   return 1;
}

int logEntity( lua_State *L )
{
   return 0;
}
