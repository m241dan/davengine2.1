/* nanny.h written by Davenge */

struct nanny_data
{
   ID_TAG *tag;
   D_SOCKET *sock;
   char *lua_path;
   int state;
   bool can_back;
};

/* creation */

NANNY_DATA *init_nanny( void );

/* deletion */

void free_nanny( NANNY_DATA *nanny ); /* nannys are not persistant or saved but do get ID tags to track active variables because I didn't want to write a seperate variable system just for nannys */

/* utility */
bool set_nPath( NANNY_DATA *nanny, const char *path );
bool set_nSocket( NANNY_DATA *nanny, D_SOCKET *socket );
bool set_nState( NANNY_DATA *nanny, int state );
bool nanny_stateNext( NANNY_DATA *nanny );
bool nanny_statePrev( NANNY_DATA *nanny );
bool onNannyInterp( NANNY_DATA *nanny, const char *input );
bool onNannyPrompt( NANNY_DATA *nanny );
