/* lua utilities header file written by Davenge */

extern lua_State *lua_handle;

void init_lua_handle			( void );
void load_lua_server_config_script	( void );

void lua_pushaccount( lua_State *L, ACCOUNT_DATA *account );
