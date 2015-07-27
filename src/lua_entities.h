int luaopen_EntityLib( lua_State *L );
int EntityGC( lua_State *L );

/* entity lib functions */
int newEntity( lua_State *L );
int getEntity( lua_State *L );
int delEntity( lua_State *L );
int logEntity( lua_State *L );
int makePlayer( lua_State *L );

/* entity object methods */

/* lua getters */
int entity_getID( lua_State *L );
int entity_getScript( lua_State *L );
int entity_getCoords( lua_State *L );
int entity_getContainedBy( lua_State *L );
int entity_getLevel( lua_State *L );

/* lua setters */
int entity_setScript( lua_State *L );
int entity_setType( lua_State *L );
int entity_setSubType( lua_State *L );

/* utility */
int entity_init( lua_State *L );
int entity_isType( lua_State *L );
int entity_isSubType( lua_State *L );
int entity_isMapped( lua_State *L );
int entity_toEntity( lua_State *L );
int entity_toggleType( lua_State *L );
int entity_toggleSubType( lua_State *L );
int entity_message( lua_State *L );
