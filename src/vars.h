/* vars.h written by Davenge */

struct lua_var
{
   ID_TAG *tag;
   TAG_TYPE ownertype;
   int ownerid;
   char *name;
};

struct lua_index
{
   LUA_VAR *owner;
   TAG_TYPE type;
   long int index;
};

struct lua_data
{
   LUA_VAR *owner;
   TAG_TYPE type;
   long int data;
};

int luaopen_VarLib( lua_State *L );
int VarGC( lua_State *L );

int newVar( lua_State *L );
int setVar( lua_State *L );
int getVar( lua_State *L );
int delVar( lua_State *L );
int newVarIndex( lua_State *L );

/* creation */
LUA_VAR		*init_var	( void );
LUA_INDEX	*init_varindex	( LUA_VAR *var );
LUA_DATA	*init_vardata	( LUA_VAR *var );
LUA_INDEX	*standard_index	( LUA_VAR *var );
bool		 new_var	( LUA_VAR *var, LUA_INDEX *index, LUA_DATA *data );
LUA_VAR		*load_var	( TAG_TYPE ownertype, int ownerid, const char *name );
int		 db_load_var	( LUA_VAR *var, MYSQL_ROW *row );

/* deletion */
void 	free_var	( LUA_VAR *var );
void 	free_varindex	( LUA_INDEX *index );
void 	free_vardata	( LUA_DATA *data );

/* getters */

LUA_VAR	*get_var_from_monitor	( TAG_TYPE ownertype, int id, const char *name );

/* utility */
bool check_exists( LUA_VAR *var );
