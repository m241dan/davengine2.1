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

