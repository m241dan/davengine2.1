#include "mud.h"

/* creation */
ACCOUNT_DATA *init_account( void )
{
   ACCOUNT_DATA *account;
   account			= malloc( sizeof( ACCOUNT_DATA ) );
   account->tag			= init_tag();
   account->managing_state	= NULL;
   account->name		= NULL;
   account->passwd		= NULL;
   account->level		= LEVEL_BASIC;
   return account;
}

ACCOUNT_DATA *load_accountByID( long long id )
{
   ACCOUNT_DATA *account;
   MYSQL_ROW tag_row;
   MYSQL_ROW account_row;
   char query[MAX_BUFFER];

   snprintf( query, MAX_BUFFER, "SELECT * FROM `accounts` WHERE accountID=%lli;", id );
   if( ( account_row = db_query_single_row( query ) ) == NULL )
      return NULL;

   snprintf( query, MAX_BUFFER, "SELECT * FROM `id_tags` WHERE type=%d AND id=%lli;", ACCOUNT_TAG, id );
   if( ( tag_row = db_query_single_row( query ) ) == NULL )
   {
      bug( "%s: could not find tag for account with ID %lli", __FUNCTION__, id );
      return NULL;
   }
   account = init_account();
   db_load_tag( account->tag, &tag_row );
   db_load_account( account, &account_row );
   FREE( tag_row );
   FREE( account_row );
   return account;
}

ACCOUNT_DATA *load_accountByName( const char *name )
{
   ACCOUNT_DATA *account;
   MYSQL_ROW tag_row;
   MYSQL_ROW account_row;
   char query[MAX_BUFFER];

   snprintf( query, MAX_BUFFER, "SELECT * FROM `accounts` WHERE name='%s';", name );
   if( ( account_row = db_query_single_row( query ) ) == NULL )
      return NULL;

   snprintf( query, MAX_BUFFER, "SELECT * FROM `id_tags` WHERE type=%d AND id=%lli;", ACCOUNT_TAG, atoll( account_row[0] ) );
   if( ( tag_row = db_query_single_row( query ) ) == NULL )
   {
      bug( "%s: could not find tag for account with ID %lli", __FUNCTION__, atoll( account_row[0] ) );
      return NULL;
   }
   account = init_account();
   db_load_tag( account->tag, &tag_row );
   db_load_account( account, &account_row );
   FREE( tag_row );
   FREE( account_row );
   return account;
}

int db_load_account( ACCOUNT_DATA *account, MYSQL_ROW *row )
{
   int counter = 1; /* start at 1 because 0 is the id which we should already have at this point */
   account->name = strdup( (*row)[counter++] );
   account->passwd = strdup( (*row)[counter++] );
   account->level = atoi( (*row)[counter++] );
   return counter;
}

int new_account( ACCOUNT_DATA *account )
{
   SET_TYPE( account, ACCOUNT_TAG );
   if( !new_tag( account->tag, "system" ) )
      return 0;
   if( GET_TYPE( account ) == ACCOUNT_TAG && GET_ID( account ) > 0 )
   {
      if( !quick_query( "INSERT INTO `accounts` VALUES ( %lli, '%s', '%s', %d );",
         GET_ID( account ), get_aName( account ), get_aPasswd( account ), get_aLevel( account ) ) )
      {
         bug( "%s: could not add new account to databse.", __FUNCTION__ );
         return 0;
      }
   }
   return 1;
}

/* deletion */
bool free_account( ACCOUNT_DATA *account )
{
   if( ListHas( active_accounts, account ) )
   {
      bug( "%s: account still apart of global monitor list, cannot free.", __FUNCTION__ );
      return FALSE;
   }
   free_state( account->managing_state );
   account->managing_state = NULL;
   free_tag( account->tag );
   account->tag = NULL;
   FREE( account->name );
   FREE( account->passwd );
   FREE( account );
   return TRUE;
}

bool delete_account( ACCOUNT_DATA *account )
{
   if( !quick_query( "DELETE FROM `accounts` WHERE accountID=%lli;", GET_ID( account ) ) )
   {
      bug( "%s: could not delete account with id %lli from db.", __FUNCTION__, GET_ID( account ) );
      return FALSE;
   }
   delete_tag( account->tag );
   free_account( account );
   return TRUE;
}
/* setters */
int set_aName( ACCOUNT_DATA *account, const char *name )
{
   FREE( account->name );
   account->name = new_string( name );
   if( GET_TYPE( account ) != TAG_UNSET && GET_ID( account ) > 0 )
      if( !quick_query( "UPDATE `accounts` SET name='%s' WHERE accountID=%lli;", account->name, GET_ID( account ) ) )
         bug( "%s: unable to update db with the new name.", __FUNCTION__ );
   return 1;
}

int set_aPasswd( ACCOUNT_DATA *account, const char *oldpw, const char *passwd )
{
   if( account->passwd && !verify_account_password( oldpw, account ) )
   {
      bug( "%s: attempting to change pw, but bad oldpw passed.", __FUNCTION__ );
      return 0;
   }
   FREE( account->passwd );
   account->passwd = new_string( crypt( passwd, account->name ) );
   if( GET_TYPE( account ) != TAG_UNSET && GET_ID( account ) > 0 )
      if( !quick_query( "UPDATE `accounts` SET password='%s' WHERE accountID=%lli;", account->passwd, GET_ID( account ) ) )
         bug( "%s: unable to update db with the new name.", __FUNCTION__ );
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
   if( GET_TYPE( account ) != TAG_UNSET && GET_ID( account ) > 0 )
      if( !quick_query( "UPDATE `accounts` SET level=%d WHERE accountID=%lli;", account->level, GET_ID( account ) ) )
         bug( "%s: unable to update db with the new name.", __FUNCTION__ );
   return 1;
}

/* getters */
ACCOUNT_DATA *get_accountByID( long long id )
{
   ACCOUNT_DATA *account;

   if( ( account = get_accountByID_ifActive( id ) ) != NULL )
      return account;

   if( ( account = load_accountByID( id ) ) == NULL )
      return NULL;

   AttachToList( account, active_accounts );
   return account;
}

ACCOUNT_DATA *get_accountByID_ifActive( long long id )
{
   ACCOUNT_DATA *account;
   ITERATOR Iter;

   AttachIterator( &Iter, active_accounts );
   while( ( account = (ACCOUNT_DATA *)NextInList( &Iter ) ) != NULL )
      if( GET_ID( account ) == id )
         break;
   DetachIterator( &Iter );
  return account;
}

ACCOUNT_DATA *get_accountByName( const char *name )
{
   ACCOUNT_DATA *account;

   if( ( account = get_accountByName_ifActive( name ) ) != NULL )
      return account;

   if( ( account = load_accountByName( name ) ) == NULL )
      return NULL;

   AttachToList( account, active_accounts );
   return account;

}

ACCOUNT_DATA *get_accountByName_ifActive( const char *name )
{
   ACCOUNT_DATA *account;
   ITERATOR Iter;

   AttachIterator( &Iter, active_accounts );
   while( ( account = (ACCOUNT_DATA *)NextInList( &Iter ) ) != NULL )
      if( !strcmp( get_aName( account ), name ) )
         break;
   DetachIterator( &Iter );
   return account;
}

const char *get_aName( ACCOUNT_DATA *account )
{
   static char buf[MAX_BUFFER];
   snprintf( buf, MAX_BUFFER, "%s", account->name );
   return buf;
}
const char *get_aPasswd( ACCOUNT_DATA *account )
{
   static char buf[MAX_BUFFER];
   snprintf( buf, MAX_BUFFER, "%s", account->passwd );
   return buf;
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
