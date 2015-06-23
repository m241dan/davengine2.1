#include "mud.h"

bool check_sql( void )
{
   if( sql_handle == NULL )
   {
      bug( "%s has found that the SQL handle is NULL.", __FUNCTION__ );
      return FALSE;
    }

   return TRUE;
}

void report_sql_error( MYSQL *con )
{
   bug( "%s: %s", __FUNCTION__, mysql_error( con ) );
   return;

}

bool quick_query( const char *format, ... )
{
   char query[MAX_BUFFER];
   va_list va;
   int res;

   va_start( va, format );
   res = vsnprintf( query, MAX_BUFFER, format, va );
   va_end( va );

   if( res >= MAX_BUFFER )
   {
      bug( "%s: received a long or unusable format: %s", __FUNCTION__, format );
      return FALSE;
   }

   if( !check_sql() )
      return FALSE;

   if( mysql_query( sql_handle, query ) )
   {
      report_sql_error( sql_handle );
      return FALSE;
   }
   return TRUE;
}

MYSQL_ROW db_query_single_row( const char *query  )
{
   MYSQL_RES *result;
   MYSQL_ROW sql_row;
   MYSQL_ROW to_return;
   int size;
   if( !quick_query( query ) )
      return NULL;
   if( ( result = mysql_store_result( sql_handle ) ) == NULL )
      return NULL;
   if( mysql_num_rows( result ) == 0 )
      return NULL;
   if( ( size = mysql_num_fields( result ) ) == 0 )
   {
      mysql_free_result( result );
      return NULL;
   }

   to_return = malloc( sizeof( MYSQL_ROW ) * size );
   sql_row = mysql_fetch_row( result );
   copy_row( &to_return, &sql_row, size );

   mysql_free_result( result );
   return to_return;
}

bool db_query_list_row( LLIST *list, const char *query )
{
   MYSQL_RES *result;
   MYSQL_ROW row_ptr;
   MYSQL_ROW row;
   int size;

   if( !list )
      return FALSE;
   if( !quick_query( query ) )
      return FALSE;
   if( ( result = mysql_store_result( sql_handle ) ) == NULL )
      return FALSE;

   size = mysql_num_fields( result );

   while( ( row = mysql_fetch_row( result ) ) != NULL )
   {
      row_ptr = malloc( sizeof( MYSQL_ROW ) * size );
      copy_row( &row_ptr, &row, size );
      AttachToList( row_ptr, list );
   }

  /* debug_row_list( list ); */

   mysql_free_result( result );
   return TRUE;
}

void copy_row( MYSQL_ROW *row_dest, MYSQL_ROW *row_src, int size )
{
   int x;

   for( x = 0; x < size; x++ )
      (*row_dest)[x] = strdup( (*row_src)[x] );
   return;
}

void debug_row( MYSQL_ROW *row, int size )
{
   char buf[MAX_BUFFER];
   char tempstring[MAX_BUFFER];
   int x;

   snprintf( buf, MAX_BUFFER, "Row Addr: %p - ", (void *)row );
   for( x = 0; x < size; x++ )
   {
      snprintf( tempstring, MAX_BUFFER, "%s, ", (*row)[x] );
      strcat( buf, tempstring );
   }

   bug( "%s: %s", __FUNCTION__, buf );
   return;
}

void debug_row_list( LLIST *list )
{
   MYSQL_ROW *row_ptr;
   ITERATOR Iter;

   AttachIterator( &Iter, list );
   while( ( row_ptr = (MYSQL_ROW *)NextInList( &Iter ) ) != NULL )
      debug_row( row_ptr, 1 );
   DetachIterator( &Iter );
}
