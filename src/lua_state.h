int luaopen_StateLib( lua_State *L );
int StateGC( lua_State *L );

/* lib functions */
int state_new( lua_State *L );

/* object methods */
int state_setControlFile( lua_State *L );
int state_control( lua_State *L );
int state_message( lua_State *L );
