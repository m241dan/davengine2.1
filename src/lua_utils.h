/* lua utilities header file written by Davenge */

extern lua_State *lua_handle;

#define DAVLUACM_ACCOUNT_NIL( account, L )                              \
do                                                                      \
{                                                                       \
   if( ( (account) = *(ACCOUNT_DATA **)luaL_checkudata( (L), 1, "Account.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushnil( (L) );                                               \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_ACCOUNT_BOOL( account, L )                             \
do                                                                      \
{                                                                       \
   if( ( (account) = *(ACCOUNT_DATA **)luaL_checkudata( (L), 1, "Account.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushboolean( (L), 0 );                                        \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_ACCOUNT_NONE( account, L )                             \
do                                                                      \
{                                                                       \
   if( ( (account) = *(ACCOUNT_DATA **)luaL_checkudata( (L), 1, "Account.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      return 0;                                                         \
   }                                                                    \
} while(0)


void init_lua_handle			( void );
void load_lua_server_config_script	( void );

void lua_pushaccount( lua_State *L, ACCOUNT_DATA *account );
