
typedef enum {
   LEVEL_BASIC, LEVEL_ADMIN, LEVEL_DEVELOPER, LEVEL_OWNER, MAX_LEVEL
} ACCT_LEVEL;

struct game_account
{
   D_SOCKET 	*socket;
   char		*name;
   char		*passwd;
   ACCT_LEVEL	 level;
};

/* creation */
ACCOUNT_DATA	*init_account( void );

/* deletion */
void		 free_account( ACCOUNT_DATA *account );

/* setters */
int 		 set_aSocket( ACCOUNT_DATA *account, D_SOCKET *socket );
int		 set_aName( ACCOUNT_DATA *account, const char *name );
int		 set_aPasswd( ACCOUNT_DATA *account, const char *passwd );
int		 set_aLevel( ACCOUNT_DATA *account, ACCT_LEVEL level );

/* getters */
D_SOCKET	*get_aSocket( ACCOUNT_DATA *account );
char		*get_aName( ACCOUNT_DATA *account );
char		*get_aPasswd( ACCOUNT_DATA *account );
ACCT_LEVEL	 get_aLevel( ACCOUNT_DATA *account );

/* utility */
bool		 verify_account_password( const char *entered_pass, ACCOUNT_DATA *account );
