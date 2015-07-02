
typedef enum {
   LEVEL_BASIC, LEVEL_ADMIN, LEVEL_DEVELOPER, LEVEL_OWNER, MAX_LEVEL
} ACCT_LEVEL;

struct game_account
{
   D_SOCKET 	*socket;
   ID_TAG	*tag;
   char		*name;
   char		*passwd;
   ACCT_LEVEL	 level;
};

/* creation */
ACCOUNT_DATA	*init_account( void );
ACCOUNT_DATA 	*load_accountByID( int id );
ACCOUNT_DATA	*load_accountByName( const char *name );
int		 db_load_account( ACCOUNT_DATA *account, MYSQL_ROW *row );
int		 new_account( ACCOUNT_DATA *account );

/* deletion */
bool		 free_account( ACCOUNT_DATA *account );
bool		 delete_account( ACCOUNT_DATA *account );

/* setters */
int 		 set_aSocket( ACCOUNT_DATA *account, D_SOCKET *socket );
int		 set_aName( ACCOUNT_DATA *account, const char *name );
int		 set_aPasswd( ACCOUNT_DATA *account, const char *oldpw, const char *passwd );
int		 set_aLevel( ACCOUNT_DATA *account, ACCT_LEVEL level );

/* getters */
ACCOUNT_DATA	*get_accountByID( int id ); /* load and attach if not already active */
ACCOUNT_DATA	*get_accountByID_ifActive( int id );
ACCOUNT_DATA	*get_accountByName( const char *name ); /* ibid */
ACCOUNT_DATA	*get_accountByName_ifActive( const char *name );
D_SOCKET	*get_aSocket( ACCOUNT_DATA *account );
const char	*get_aName( ACCOUNT_DATA *account );
const char	*get_aPasswd( ACCOUNT_DATA *account );
ACCT_LEVEL	 get_aLevel( ACCOUNT_DATA *account );

/* utility */
bool verify_account_password( const char *entered_pass, ACCOUNT_DATA *account );

