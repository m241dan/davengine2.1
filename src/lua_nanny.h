int luaopen_NannyLib( lua_State *L );
int NannyGC( lua_State *L );

/* Nanny Lib Functions */
int newNanny( lua_State *L );

/* nanny object methods */

/* getters */
int nanny_getState( lua_State *L );
/* setters */
int nanny_setState( lua_State *L );

/* utility */
int nanny_stateNext( lua_State *L );
int nanny_statePrev( lua_State *L );

