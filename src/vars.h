/* vars.h written by Davenge */

extern const char *const settable_vars[];

#define MAX_VINDEX 60

struct lua_var
{
   ID_TAG *tag;
   TAG_TYPE ownertype;
   int ownerid;
   char *name;
};

struct lua_index
{
   TAG_TYPE type;
   char index[MAX_VINDEX];
};

struct lua_data
{
   TAG_TYPE type;
   char data[MAX_BUFFER];
   bool isscript;
};

int luaopen_VarLib( lua_State *L );
int VarGC( lua_State *L );

int newVar( lua_State *L );
int setVar( lua_State *L );
int getVar( lua_State *L );
int delVar( lua_State *L );
int newVarIndex( lua_State *L );
int getVarIndex( lua_State *L );
int getVarOwner( lua_State *L );
int setScript( lua_State *L );

/* creation */
LUA_VAR		*init_var	( void );
void		 standard_index	( LUA_INDEX *index );
bool		 new_var	( LUA_VAR *var, LUA_INDEX *index, LUA_DATA *data );
LUA_VAR		*load_var	( TAG_TYPE ownertype, int ownerid, const char *name );
int		 db_load_var	( LUA_VAR *var, MYSQL_ROW *row );
int		 db_load_data	( LUA_DATA *data, MYSQL_ROW *row );

/* deletion */
void 	free_var		( LUA_VAR *var );
void	delete_var_from_db	( LUA_VAR *var );
void	delete_index_from_db	( LUA_VAR *var, LUA_INDEX *index );

/* getters */
int	get_global_var_id	( const char *name );
LUA_VAR	*get_var_from_monitor	( TAG_TYPE ownertype, int id, const char *name );

/* utility */
bool check_exists( LUA_VAR *var );
bool varsettable( lua_State *L, int index );
bool lua_pushvarowner( lua_State *L, LUA_VAR *var );
