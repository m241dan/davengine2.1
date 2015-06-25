#include "mud.h"

/* creation */
ACCOUNT_DATA *init_account( void )
{
   ACCOUNT_DATA *account;
   account		= malloc( sizeof( ACCOUNT_DATA ) );
   account->tag		= init_tag();
   account->socket	= NULL;
   account->name	= NULL;
   account->passwd	= NULL;
   account->level	= LEVEL_BASIC;
   return account;
}

ACCOUNT_DATA *load_accountByID( int id )
{
   ACCOUNT_DATA *account;
   MYSQL_ROW tag_row;
   MYSQL_ROW account_row;
   char query[MAX_BUFFER];

   snprintf( query, MAX_BUFFER, "SELECT * FROM `accounts` WHERE accountID=%d;", id );
   if( ( account_row = db_query_single_row( query ) ) == NULL )
      return NULL;

   snprintf( query, MAX_BUFFER, "SELECT * FROM `id_tags` WHERE type=%d AND id=%d;", ACCOUNT_TAG, id );
   if( ( tag_row = db_query_single_row( query ) ) == NULL )
   {
      bug( "%s: could not find tag for account with ID %d", __FUNCTION__, id );
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

   snprintf( query, MAX_BUFFER, "SELECT * FROM `id_tags` WHERE type=%d AND id=%d;", ACCOUNT_TAG, atoi( account_row[0] ) );
   if( ( tag_row = db_query_single_row( query ) ) == NULL )
   {
      bug( "%s: could not find tag for account with ID %d", __FUNCTION__, atoi( account_row[0] ) );
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

/* deletion */
void free_account( ACCOUNT_DATA *account )
{
   DetachFromList( account, active_accounts ); /* if we're going to free this data, make sure it's not attached to the active list */
   account->socket = NULL;
   free_tag( account->tag );
   FREE( account->name );
   FREE( account->passwd );
   FREE( account );
}

bool delete_account( ACCOUNT_DATA *account )
{
   if( !quick_query( "DELETE FROM `accounts` WHERE accountID=%d;", GET_ID( account ) ) )
   {
      bug( "%s: could not delete account with id %d from db.", __FUNCTION__, GET_ID( account ) );
      return FALSE;
   }
   if( !quick_query( "DELETE FROM `id_tags` WHERE type=%d and id=%d;", GET_TYPE( account ), GET_ID( account ) ) )
   {
      bug( "%s: could not delete account with id %d tag from db.", __FUNCTION__, GET_ID( account ) );
      return FALSE;
   }
   free_account( account );
   return TRUE;
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
   if( GET_TYPE( account ) != TAG_UNSET && GET_ID( account ) > 0 )
      if( !quick_query( "UPDATE `accounts` SET name='%s' WHERE accountID=%d;", account->name, GET_ID( account ) ) )
         bug( "%s: unable to update db with the new name.", __FUNCTION__ );
   return 1;
}

int set_aPasswd( ACCOUNT_DATA *account, const char *passwd )
{
   FREE( account->passwd );
   account->passwd = strdup( crypt( passwd, account->name ) );
   if( GET_TYPE( account ) != TAG_UNSET && GET_ID( account ) > 0 )
      if( !quick_query( "UPDATE `accounts` SET password='%s' WHERE accountID=%d;", account->passwd, GET_ID( account ) ) )
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
      if( !quick_query( "UPDATE `accounts` SET level=%d WHERE accountID=%d;", account->level, GET_ID( account ) ) )
         bug( "%s: unable to update db with the new name.", __FUNCTION__ );
   return 1;
}

/* getters */
ACCOUNT_DATA *get_accountByID( int id )
{
   ACCOUNT_DATA *account;
   ITERATOR Iter;

   AttachIterator( &Iter, active_accounts );
   while( ( account = (ACCOUNT_DATA *)NextInList( &Iter ) ) != NULL )
      if( GET_ID( account ) == id )
         break;
   DetachIterator( &Iter );

   if( account )
      return account;

   if( ( account = load_accountByID( id ) ) == NULL )
      return NULL;

   AttachToList( account, active_accounts );
   return account;
}

ACCOUNT_DATA *get_accountByName( const char *name )
{
   ACCOUNT_DATA *account;
   ITERATOR Iter;

   AttachIterator( &Iter, active_accounts );
   while( ( account = (ACCOUNT_DATA *)NextInList( &Iter ) ) != NULL )
      if( !strcmp( get_aName( account ), name ) )
         break;
   DetachIterator( &Iter );

   if( account )
      return account;

   if( ( account = load_accountByName( name ) ) == NULL )
      return NULL;

   AttachToList( account, active_accounts );
   return account;

}

D_SOCKET *get_aSocket( ACCOUNT_DATA *account )
{
   return account->socket;
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
