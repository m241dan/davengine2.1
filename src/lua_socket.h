/* lua_socket written by Davenge to give lua access to parts of the socket */

int getSocket( lua_State *L );

/* setters */
int setState( lua_State *L );

/* getters */
int getState( lua_State *L );

/* utility */
int nextState( lua_State *L );
int prevState( lua_State *L );
