/* id-handlers.c written by Davenge */

#include "mud.h"

ID_TAG *init_tag( void )
{
   ID_TAG *tag;

   tag = calloc( sizeof( ID_TAG ), 1 );
   if( !clear_tag( tag ) )
   {
      free_tag( tag );
      return NULL;
   }
   return tag;
}

int clear_tag( ID_TAG *tag )
{
   if( !tag )
      return 0;

   tag->type = TAG_UNSET;
   tag->id = 0;
   tag->can_recycle = FALSE;
   FREE( tag->created_by );
   FREE( tag->created_on );
   FREE( tag->modified_by );
   FREE( tag->modified_on );
   tag->created_by = new_string( NULL );
   tag->created_on = new_string( strip_nl( ctime( &current_time ) ) );
   tag->modified_by = new_string( NULL );
   tag->modified_on = new_string( strip_nl( ctime( &current_time ) ) );

   return 1;
}

int free_tag( ID_TAG *tag )
{
   FREE( tag->created_on );
   FREE( tag->created_by );
   FREE( tag->modified_by );
   FREE( tag->modified_on );
   FREE( tag );
   return 1;
}

int delete_tag( ID_TAG *tag )
{
   if( tag->can_recycle )
      if( !quick_query( "INSERT INTO `id_recycled` VALUES( '%d', '%lli' );", tag->type, tag->id ) )
         bug( "%s: did not update recycled ids database with tag %d of type %d.", __FUNCTION__, tag->id, tag->type );
   if( !quick_query( "DELETE FROM `id_tags` WHERE type=%d and id=%lli;", tag->type, tag->id ) )
      bug( "%s: could not delete tag %d of type %d from the master tag table.", __FUNCTION__, tag->id, tag->type );
   if( !quick_query( "DELETE FROM `vars` WHERE ownertype=%d AND ownerid=%lli;", tag->type, tag->id ) )
      bug( "%s: could not delete vars of ownerid %lli of ownertype %d.", __FUNCTION__, tag->id, tag->type );
   free_tag( tag );
   return 1;
}

int new_tag( ID_TAG *tag, const char *creator )
{
   if( tag->type == TAG_UNSET )
   {
      bug( "%s: tag has bad type: %d", __FUNCTION__, tag->type );
      return 0;
   }
   tag->id = get_new_id( tag->type );
   tag->can_recycle = can_tag_be_recycled( tag->type );
   tag->created_by = strdup( creator );
   tag->modified_by = strdup( creator );
   tag->created_on = strdup( strip_nl( ctime( &current_time ) ) );
   tag->modified_on = strdup( strip_nl( ctime( &current_time ) ) );
   if( !quick_query( "INSERT INTO `id_tags` VALUES( %d, %d, %d, '%s', '%s', '%s', '%s' );",
      tag->type, tag->id, (int)tag->can_recycle, tag->created_by, tag->created_on, tag->modified_by, tag->modified_on ) )
   {
      bug( "%s: tag could not be added to the DB.", __FUNCTION__ );
      return 0;
   }
   return 1;
}

int db_load_tag( ID_TAG *tag, MYSQL_ROW *row )
{
   int counter = 0;

   tag->type = atoi( (*row)[counter++] );
   tag->id = atoll( (*row)[counter++] );
   tag->can_recycle = atoi( (*row)[counter++] );
   tag->created_by = strdup( (*row)[counter++] );
   tag->created_on = strdup( (*row)[counter++] );
   tag->modified_by = strdup( (*row)[counter++] );
   tag->modified_on = strdup( (*row)[counter++] );

   return counter;
}

int update_tag( ID_TAG *tag, const char *effector, ... )
{
   char buf[50];
   va_list va;
   int res;

   if( !strcmp( tag->created_by, "null" ) )
      return 0;

   va_start( va, effector );
   res = vsnprintf( buf, 50, effector, va );
   va_end( va );

   if( res >= 50 - 1 )
   {
      bug( "%s: effector name too long. Length produced = %d", __FUNCTION__, res );
      return 0;
   }

   tag->modified_by = new_string( buf );
   tag->modified_on = new_string( strip_nl( ctime( &current_time ) ) );

   if( !quick_query( "UPDATE `id_tags` SET modified_by='%s', modified_on='%s' WHERE type=%d and id=%d;", 
      tag->modified_by, tag->modified_on, tag->type, tag->id ) )
      bug( "%s: could not save the update to the database.", __FUNCTION__ );

   return 1;
}

long long get_new_id( TAG_TYPE type )
{
   long long new_id = 0;

   if( can_tag_be_recycled( type ) && ( new_id = get_recycled_id( type ) ) != -1 )
   {
      if( !quick_query( "DELETE FROM `id_recycled` WHERE rec_id=%lli and type=%d;", new_id, type ) )
         bug( "%s: could not delete recycled ID from id_recycled table", __FUNCTION__ );
   }
   else if( ( new_id = get_potential_id( type ) ) != 0 )
   {
      if( !quick_query( "UPDATE `id_handlers` SET top_id=%lli WHERE type=%d;", ( new_id + 1 ), type ) )
         bug( "%s: could not update database with new Top ID", __FUNCTION__ );
   }
   else
   {
      new_id = 0;
      bug( "%s: could not get proper new ID", __FUNCTION__ );
   }
   return new_id;
}

long long get_potential_id( TAG_TYPE type )
{
   MYSQL_ROW row;
   char query[MAX_BUFFER];
   int potential;

   if( !can_tag_be_recycled( type ) || ( potential = get_recycled_id( type ) ) == 0 )
   {
      snprintf( query, MAX_BUFFER, "SELECT top_id FROM `id_handlers` WHERE type=%d;", type );
      if( ( row = db_query_single_row( query ) ) == NULL )
      {
         bug( "%s: db_query_single_row returned NULL.", __FUNCTION__ );
         return 0;
      }
      potential = atoll( row[0] );
      free( row );
   }
   return potential;
}

bool can_tag_be_recycled( TAG_TYPE type )
{
   MYSQL_ROW row;
   char query[MAX_BUFFER];
   bool recycle;

   snprintf( query, MAX_BUFFER, "SELECT can_recycle FROM `id_handlers` WHERE type=%d;", type );
   if( ( row = db_query_single_row( query ) ) == NULL )
   {
      bug( "%s: db_query_single_row returned NULL.", __FUNCTION__ );
      return 0;
   }
   recycle = (bool)atoi( row[0] );
   free( row );
   return recycle;
}

long long get_recycled_id( TAG_TYPE type )
{
   MYSQL_ROW row;
   char query[MAX_BUFFER];
   long long rec_id;

   snprintf( query, MAX_BUFFER, "SELECT rec_id FROM `id_recycled` WHERE type=%d LIMIT 1;", type );
   if( ( row = db_query_single_row( query ) ) == NULL )
      return 0;

   rec_id = atoll( row[0] );
   free( row );
   return rec_id;
}

ID_TAG *copy_tag( ID_TAG *tag )
{
   ID_TAG *tag_copy;

   if( !tag )
   {
      bug( "%s: passed a NULL tag.", __FUNCTION__ );
      return NULL;
   }

   tag_copy = init_tag();
   tag_copy->type = tag->type;
   tag_copy->id = tag->id;
   tag_copy->can_recycle = tag->can_recycle;
   tag_copy->created_by = strdup( tag->created_by );
   tag_copy->created_on = strdup( tag->created_on );
   tag_copy->modified_by = strdup( tag->modified_by );
   tag_copy->modified_on = strdup( tag->modified_on );

   return tag_copy;
}

