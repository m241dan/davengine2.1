#include "mud.h"

const struct luaL_Reg EntityLib_m[] = {
   /* getters */
   { "getID", entity_getID },
   { "getScript", entity_getScript },
   { "getCoords", entity_getCoords },
   { "getContainedBy", entity_getContainedBy },
   /* setters */
   { "setScript", entity_setScript },
   { "setType", entity_setType },
   { "setSubType", entity_setSubType },
   /* utility */
   { "isType", entity_isType },
   { "isSubType", entity_isSubType },
   { "isMapped", entity_isMapped },
   { "to", entity_toEntity },
   { "toggleType", entity_toggleType },
   { "toggleSubType", entity_toggleSubType },
   { NULL, NULL }
};

const struct luaL_Reg EntityLib_f[] = {
   { "new", newEntity },
   { "get", getEntity },
   { "del", delEntity },
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
   set_eScript( entity, lua_tostring( L, 1 ) );
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

/* lua getters */
int entity_getID( lua_State *L )
{
   ENTITY_DATA *entity;

   DAVLUACM_ENTITY_NIL( entity, L );

   if( VALID_TAG( entity ) )
      lua_pushnumber( L, GET_ID( entity ) );
   else
      lua_pushnil( L );

   return 1;
}

int entity_getScript( lua_State *L )
{
   ENTITY_DATA *entity;

   DAVLUACM_ENTITY_NIL( entity, L );

   if( VALID_TAG( entity ) )
      lua_pushstring( L, entity->script );
   else
      lua_pushnil( L );

   return 1;
}

int entity_getCoords( lua_State *L )
{
   ENTITY_DATA *entity;

   DAVLUACM_ENTITY_NIL( entity, L );

   if( VALID_TAG( entity ) )
   {
      lua_pushnumber( L, entity->coord.x );
      lua_pushnumber( L, entity->coord.y );
      lua_pushnumber( L, entity->coord.z );
   }
   else
   {
      lua_pushnil( L );
      lua_pushnil( L );
      lua_pushnil( L );
   }

   return 3;
}

int entity_getContainedBy( lua_State *L )
{
   ENTITY_DATA *entity;

   DAVLUACM_ENTITY_NIL( entity, L );

   if( VALID_TAG( entity ) && entity->contained_by )
      lua_pushobj( L, entity, ENTITY_DATA );
   else
      lua_pushnil( L );

   return 1;
}

/* lua setters */
int entity_setScript( lua_State *L )
{
   ENTITY_DATA *entity;
   char script[MAX_BUFFER];

   DAVLUACM_ENTITY_BOOL( entity, L );
   if( lua_type( L, 2 ) != LUA_TSTRING )
   {
      bug( "%s: argument 1 expect type string.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   snprintf( script, MAX_BUFFER, "%s", lua_tostring( L, 2 ) );
   if( !file_exists( script ) )
   {
      bug( "%s: script file %s does not exist.", __FUNCTION__, script );
      lua_pushboolean( L, 0 );
      return 1;
   }
   set_eScript( entity, script );
   lua_pushboolean( L, 1 );
   return 1;
}

int entity_setType( lua_State *L )
{
   ENTITY_DATA *entity;
   int type;

   DAVLUACM_ENTITY_BOOL( entity, L );
   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: argument 1 expected to be of type number.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   type = lua_tonumber( L, 2 );
   if( type < 0 || type >= MAX_ENTITY_TYPE )
   {
      bug( "%s: the type number passed is out of range.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   set_eType( entity, (ENTITY_TYPE)type );
   lua_pushboolean( L, 0 );
   return 1;
}

int entity_setSubType( lua_State *L )
{
   ENTITY_DATA *entity;
   int stype;

   DAVLUACM_ENTITY_BOOL( entity, L );
   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: argument 1 expected to be of type number.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   stype = lua_tonumber( L, 2 );
   if( stype < 0 || stype >= MAX_ENTITY_SUB_TYPE )
   {
      bug( "%s: the type number passed is out of range.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   set_eSubType( entity, (ENTITY_TYPE)stype );
   lua_pushboolean( L, 0 );
   return 1;

}

/* utility */
int entity_isType( lua_State *L )
{
   ENTITY_DATA *entity;
   int type;

   DAVLUACM_ENTITY_BOOL( entity, L );
   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: need to pass a type as an integer.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   type = lua_tonumber( L, 2 );
   if( type < 0 || type >= MAX_ENTITY_TYPE )
   {
      bug( "%s: the type number passed is out of range.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   lua_pushboolean( L, entity->type[type] );
   return 1;
}

int entity_isSubType( lua_State *L )
{
   ENTITY_DATA *entity;
   int stype;

   DAVLUACM_ENTITY_BOOL( entity, L );
   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: need to pass a type as an integer.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   stype = lua_tonumber( L, 2 );
   if( stype < 0 || stype >= MAX_ENTITY_SUB_TYPE )
   {
      bug( "%s: the type number passed is out of range.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   lua_pushboolean( L, entity->subtype[stype] );
   return 1;
}

int entity_isMapped( lua_State *L )
{
   ENTITY_DATA *entity;

   DAVLUACM_ENTITY_BOOL( entity, L );
   lua_pushboolean( L, entity->ismapped );
   return 1;
}

int entity_toEntity( lua_State *L )
{
   ENTITY_DATA *entity;
   ENTITY_DATA *send_to;

   DAVLUACM_ENTITY_BOOL( entity, L );
   if( ( send_to = (ENTITY_DATA *)check_meta( L, 2, "Entity.meta" ) ) == NULL )
   {
      bug( "%s: argument 1 needs to be of type USERDATA with the Entity.meta metatable.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   entity_to_container( entity, send_to );
   update_position( entity );
   lua_pushboolean( L, 1 );
   return 1;
}

int entity_toggleType( lua_State *L )
{
   ENTITY_DATA *entity;
   int type;

   DAVLUACM_ENTITY_BOOL( entity, L );
   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: need to pass a type as an integer.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   type = lua_tonumber( L, 2 );
   if( type < 0 || type >= MAX_ENTITY_TYPE )
   {
      bug( "%s: the type number passed is out of range.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   toggle_eType( entity, (ENTITY_TYPE)type );
   lua_pushboolean( L, 1 );
   return 1;
}

int entity_toggleSubType( lua_State *L )
{
   ENTITY_DATA *entity;
   int stype;

   DAVLUACM_ENTITY_BOOL( entity, L );
   if( lua_type( L, 2 ) != LUA_TNUMBER )
   {
      bug( "%s: need to pass a type as an integer.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }
   stype = lua_tonumber( L, 2 );
   if( stype < 0 || stype >= MAX_ENTITY_SUB_TYPE )
   {
      bug( "%s: the sub type number passed is out of range.", __FUNCTION__ );
      lua_pushboolean( L, 0 );
      return 1;
   }

   toggle_eSubType( entity, (ENTITY_TYPE)stype );
   lua_pushboolean( L, 1 );
   return 1;
}
