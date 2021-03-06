/* lua utilities header file written by Davenge */

extern lua_State *lua_handle;
extern const char *const meta_types[MAX_TAG_TYPE+1];

#define prep_stack( path, func ) prep_stack_handle( lua_handle, (path), (func) )
#define lua_pushobj( handle, pushobj, type )				\
do									\
{									\
   type **box;							\
   if( !(pushobj) )								\
   {									\
      bug( "%s: attempting to push NULL obj; pushed a nil instead.", __FUNCTION__ );	\
      lua_pushnil( (lua_handle) );							\
      break;								\
   }									\
   box = (type **)lua_newuserdata( (lua_handle), sizeof( type * ) );		\
   luaL_getmetatable( (lua_handle), meta_types[GET_TYPE((pushobj))] );			\
   if( lua_isnil( (lua_handle), -1 ) )						\
   {									\
      bug( "%s: %s is missing.", __FUNCTION__, meta_types[GET_TYPE((pushobj))] );	\
      lua_pop( (lua_handle), 2 );							\
      lua_pushnil( (lua_handle) );							\
      break;								\
   }									\
   lua_setmetatable( (lua_handle), -2 );						\
   *box = (pushobj);							\
} while(0)

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

#define DAVLUACM_VAR_NIL( var, L )                              \
do                                                                      \
{                                                                       \
   if( ( (var) = *(LUA_VAR**)luaL_checkudata( (L), 1, "Var.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushnil( (L) );                                               \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_VAR_BOOL( var, L )                             \
do                                                                      \
{                                                                       \
   if( ( (var) = *(LUA_VAR**)luaL_checkudata( (L), 1, "Var.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushboolean( (L), 0 );                                        \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_VAR_NONE( var, L )                             \
do                                                                      \
{                                                                       \
   if( ( (var) = *(LUA_VAR**)luaL_checkudata( (L), 1, "Var.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      return 0;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_SOCKET_NIL( socket, L )                              \
do                                                                      \
{                                                                       \
   if( ( (socket) = *(D_SOCKET **)luaL_checkudata( (L), 1, "Socket.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushnil( (L) );                                               \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_SOCKET_BOOL( socket, L )                             \
do                                                                      \
{                                                                       \
   if( ( (socket) = *(D_SOCKET **)luaL_checkudata( (L), 1, "Socket.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushboolean( (L), 0 );                                        \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_SOCKET_NONE( socket, L )                             \
do                                                                      \
{                                                                       \
   if( ( (socket) = *(D_SOCKET **)luaL_checkudata( (L), 1, "Socket.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      return 0;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_STATE_NIL( state, L )                              \
do                                                                      \
{                                                                       \
   if( ( (state) = *(SOCKET_STATE **)luaL_checkudata( (L), 1, "State.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushnil( (L) );                                               \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_STATE_BOOL( state, L )                             \
do                                                                      \
{                                                                       \
   if( ( (state) = *(SOCKET_STATE **)luaL_checkudata( (L), 1, "State.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushboolean( (L), 0 );                                        \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_STATE_NONE( state, L )                             \
do                                                                      \
{                                                                       \
   if( ( (state) = *(SOCKET_STATE **)luaL_checkudata( (L), 1, "State.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      return 0;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_ENTITY_NIL( entity, L )                              \
do                                                                      \
{                                                                       \
   if( ( (entity) = *(ENTITY_DATA **)luaL_checkudata( (L), 1, "Entity.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushnil( (L) );                                               \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_ENTITY_BOOL( entity, L )                             \
do                                                                      \
{                                                                       \
   if( ( (entity) = *(ENTITY_DATA **)luaL_checkudata( (L), 1, "Entity.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      lua_pushboolean( (L), 0 );                                        \
      return 1;                                                         \
   }                                                                    \
} while(0)

#define DAVLUACM_ENTITY_NONE( entity, L )                             \
do                                                                      \
{                                                                       \
   if( ( (entity) = *(ENTITY_DATA **)luaL_checkudata( (L), 1, "Entity.meta" ) ) == NULL ) \
   {                                                                    \
      bug( "%s: bad meta table.", __FUNCTION__ );                       \
      return 0;                                                         \
   }                                                                    \
} while(0)

void init_lua_handle			( void );
void load_lua_server_config_script	( void );

bool prep_stack_handle( lua_State *handle, const char *file, const char *function );
void *check_meta( lua_State *L, int index, const char *meta_name );
int  get_meta_type_id( lua_State *L, int index );
int  get_meta_id( lua_State *L, int index );
void lua_pushstateobj( lua_State *L, SOCKET_STATE *state );

void lua_onConnect( D_SOCKET *socket );
