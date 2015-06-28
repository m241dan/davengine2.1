/* vars.h written by Davenge */

extern LLIST *lua_vars;

struct lua_var
{
   TAG_TYPE type;
   int id;
   char *name;
};

struct lua_index
{
   TAG_TYPE type;
   char *index;
}

struct lua_data
{
   TAG_TYPE type;
   char *data;
}
