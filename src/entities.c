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
   for( int x = 0; x < MAX_ENTITY_HASH; x++ )
      entity->inventory_qs[x] = AllocList();

   return entity;
}

bool new_entity( ENTITY_DATA *entity )
{
   return FALSE;
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
   for( int x = 0; x < MAX_ENTITY_HASH; x++ )
   {
      clearlist( entity->inventory_qs[x] );
      entity->inventory_qs[x] = NULL;
   }
   FREE( entity );
}

bool delete_entity( ENTITY_DATA *entity )
{
   return FALSE;
}

