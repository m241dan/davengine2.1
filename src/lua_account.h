int luaopen_AccountLib( lua_State *L );
int AccountGC( lua_State *L );

/* account lib functions */
int newAccount( lua_State *L );
int getAccount( lua_State *L );
int checkExistsAccount( lua_State *L );
int delAccount( lua_State *L );
int logAccount( lua_State *L );

/* account object methods */

/* lua setters */

int account_setName( lua_State *L );
int account_setPasswd( lua_State *L );

/* lua getters */

int account_getID( lua_State *L );
int account_getName( lua_State *L );
int account_getLevel( lua_State *L );

/* utility */

int account_init( lua_State *L );
int account_verifyPasswd( lua_State *L );
int account_addChar( lua_State *L );
int account_Message( lua_State *L );
