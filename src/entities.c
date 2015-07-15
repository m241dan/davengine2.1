	#include "mud.h"

/* creation */

ENTITY_DATA *init_entity( void )
{
   ENTITY_DATA *entity;

   entity 			= malloc( sizeof( ENTITY_DATA ) );
   entity->tag			= init_tag();
   entity->managing_state 	= NULL;
   entity->script		= NULL;
   entity->inventory		= AllocList();
   for( int x = 0; x < ENTITY_HASH; x++ )
      entity->inventory_qs[x] = AllocList();

   return entity;
}

ENTITY_DATA *load_entityByID( int id )
{
   ENTITY_DATA *entity;
   MYSQL_ROW tag_row;
   MYSQL_ROW entity_row;
   char query[MAX_BUFFER];

   snprintf( query, MAX_BUFFER, "SELECT * FROM `entities` WHERE entityID=%d;", id );
   if( ( entity_row = db_query_single_row( query ) ) == NULL )
      return NULL;

   snprintf( query, MAX_BUFFER, "SELECT * FROM `id_tags` WHERE type=%d AND id=%d;", ENTITY_TAG, id );
   if( ( tag_row = db_query_single_row( query ) ) == NULL )
   {
      bug( "%s: could not find tag for entity with ID %d", __FUNCTION__, id );
      return NULL;
   }

   entity = init_entity();
   db_load_tag( entity->tag, &tag_row );
   db_load_entity( entity, &entity_row );
   FREE( tag_row );
   FREE( entity_row );
   return entity;
}

int db_load_entity( ENTITY_DATA *entity, MYSQL_ROW *row )
{
   int counter = 0;
   entity->script = strdup( (*row)[counter++] );
   string_to_bool_array( (*row)[counter++], entity->type, MAX_ENTITY_TYPE );
   string_to_bool_array( (*row)[counter++], entity->subtype, MAX_ENTITY_SUB_TYPE );
   entity->contained_by = atoi( (*row)[counter++] );
   return counter;
}

bool new_entity( ENTITY_DATA *entity )
{
   char type[256];
   char subtype[256];

   SET_TYPE( entity, ENTITY_TAG );
   if( !new_tag( entity->tag, "system" ) )
      return FALSE;

   if( GET_TYPE( entity ) == ENTITY_TAG && GET_ID( entity ) > 0 )
   {
      snprintf( type, 256, "%s", bool_array_to_string( entity->type, MAX_ENTITY_TYPE ) );
      snprintf( subtype, 256, "%s", bool_array_to_string( entity->subtype, MAX_ENTITY_SUB_TYPE ) );
      if( !quick_query( "INSERT INTO `entities` VALUES( '%d', '%s', '%s', '%s', '%d' );",
         GET_ID( entity ), entity->script, type, subtype, entity->contained_by ? GET_ID( entity->contained_by ) : 0 ) )
      {
         bug( "%s: could not add new entity to database.", __FUNCTION__ );
         return 0;
      }
   }
   return TRUE;
}

void free_entity( ENTITY_DATA *entity )
{
   free_tag( entity->tag );
   entity->tag = NULL;
   free_state( entity->managing_state );
   entity->managing_state = NULL;
   FREE( entity->script );

   clearlist( entity->inventory );
   entity->inventory = NULL;
   for( int x = 0; x < ENTITY_HASH; x++ )
   {
      clearlist( entity->inventory_qs[x] );
      entity->inventory_qs[x] = NULL;
   }
   FREE( entity );
}

bool delete_entity( ENTITY_DATA *entity )
{
   if( !quick_query( "DELETE FROM `entities` WHERE entityID=%d;", GET_ID( entity ) ) )
   {
      bug( "%s: could not delete entity with id %d from db.", __FUNCTION__, GET_ID( entity ) );
      return FALSE;
   }
   delete_tag( entity->tag );
   free_entity( entity );
   return FALSE;
}

/* getters */
ENTITY_DATA *get_entityByID( int id )
{
   ENTITY_DATA *container;
   ENTITY_DATA *entity;

   if( ( entity = get_entityByID_ifActive( id ) ) != NULL )
      return entity;

   if( ( entity = load_entityByID( id ) ) == NULL )
      return NULL;

   AttachToList( entity, active_entities[id % ENTITY_HASH] );
   if( ( container = get_entityByID_ifActive( entity->contained_byID ) ) != NULL )
      entity_to_container( entity, container );
   return entity;
}

ENTITY_DATA *get_entityByID_ifActive( int id )
{
   ENTITY_DATA *entity;
   ITERATOR Iter;

   AttachIterator( &Iter, active_entities[id % ENTITY_HASH] );
   while( ( entity = (ENTITY_DATA *)NextInList( &Iter ) ) != NULL )
      if( GET_ID( entity ) == id )
         break;
   DetachIterator( &Iter );
   return entity;
}

const char *entity_getScript( ENTITY_DATA *entity )
{
   return NULL;
}

/* setters */
bool entity_setScript( ENTITY_DATA *entity, const char *script )
{
   return FALSE;
}
bool entity_settype( ENTITY_DATA *entity, ENTITY_TYPE type )
{
   /* handle inventory_qs if contained */
   return FALSE;
}

/* utility */
bool entity_from_its_container( ENTITY_DATA *entity )
{
   ENTITY_CONTAINER *container;

   if( !entity )
   {
      bug( "%s: trying to 'from' a NULL entity.", __FUNCTION__ );
      return FALSE;
   }

   if( ( container = entity->contained_by ) == NULL )
      return TRUE;

   DetachFromList( entity, container->inventory );
   for( int x = 0; x < MAX_ENTITY_TYPE; x++ )
      if( entity[x] == TRUE )
         DetachFromList( entity, container->inventory_qs[x] );

   return TRUE;
}

bool entity_to_container( ENTITY_DATA *entity, ENTITY_DATA *container )
{
   if( !entity )
   {
      bug( "%s: trying to 'to' a NULL entity.", __FUNCTION__ );
      return FALSE;
   }
   if( !container )
   {
      bug( "%s: triyng to 'to' an entity to a NULL container.", __FUNCTION__ );
      return FALSE;
   }

   entity_from_its_container( entity );
   
}
