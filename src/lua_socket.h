/* lua_socket written by Davenge to give lua access to parts of the socket */

int luaopen_SocketLib( lua_State *L );
int SocketGC( lua_State *L );
int getSocket( lua_State *L );

/* setters */
int socket_lsetState( lua_State *L );

/* getters */
int socket_getState( lua_State *L );

/* utility */
int socket_Control( lua_State *L );
int socket_Message( lua_State *L );
