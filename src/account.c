#include "mud.h"

/* creation */
ACCOUNT_DATA *init_account( void )
{
   ACCOUNT_DATA *account;
   account		= malloc( sizeof( ACCOUNT_DATA ) );
   account->socket	= NULL;
   account->name	= NULL;
   account->passwd	= NULL;
   account->level	= LEVEL_BASIC;
   return account;
}

/* deletion */
void free_account( ACCOUNT_DATA *account )
{
   account->socket = NULL;
   FREE( account->name );
   FREE( account->passwd );
   FREE( account );
}

/* setters */
int set_aSocket( ACCOUNT_DATA *account, D_SOCKET *socket )
{
   account->socket = socket;
   socket->account = account;
   return 1;
}

int set_aName( ACCOUNT_DATA *account, const char *name )
{
   FREE( account->name );
   account->name = strdup( name );
   return 1;
}

int set_aPasswd( ACCOUNT_DATA *account, const char *passwd )
{
   FREE( account->passwd );
   account->passwd = strdup( crypt( passwd, account->name ) );
   return 1;
}
int set_aLevel( ACCOUNT_DATA *account, ACCT_LEVEL level )
{
   if( level < LEVEL_BASIC || level > LEVEL_OWNER )
   {
      bug( "%s: level %d OUT OF BOUNDS", __FUNCTION__, level );
      return 0;
   }
   account->level = level;
   return 1;
}

/* getters */
D_SOCKET *get_aSocket( ACCOUNT_DATA *account )
{
   return account->socket;
}
char *get_aName( ACCOUNT_DATA *account )
{
   return account->name;
}
char *get_aPasswd( ACCOUNT_DATA *account )
{
   return account->passwd;
}
ACCT_LEVEL get_aLevel( ACCOUNT_DATA *account )
{
   return account->level;
}

/* utility */
bool verify_account_password( const char *entered_pass, ACCOUNT_DATA *account )
{
   if( !strcmp( crypt( entered_pass, account->name ), account->passwd ) )
      return TRUE;
   return FALSE;
}
