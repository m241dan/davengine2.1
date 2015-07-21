/* nanny.h written by Davenge */

extern LLIST *active_nannys;

struct nanny_data
{
   ID_TAG *tag;
   SOCKET_STATE *managing_state;
   char *lua_path;
   int state;
   bool can_back;
};

/* creation */

NANNY_DATA *init_nanny( void );
bool load_nanny( NANNY_DATA *nanny ); /* nanny must have a set path, then it will load any variables it needs ahead of time based on its lua script */

/* deletion */

void free_nanny( NANNY_DATA *nanny ); /* nannys are not persistant or saved but do get ID tags to track active variables because I didn't want to write a seperate variable system just for nannys */

/* getters */
NANNY_DATA *get_nannyByID_ifActive( long long id );

/* utility */
bool set_nPath( NANNY_DATA *nanny, const char *path );
bool set_nSocket( NANNY_DATA *nanny, D_SOCKET *socket );
bool set_nState( NANNY_DATA *nanny, int state );
bool state_nNext( NANNY_DATA *nanny );
bool state_nPrev( NANNY_DATA *nanny );
bool onNannyInterp( NANNY_DATA *nanny, const char *input );
bool onNannyPrompt( NANNY_DATA *nanny );
