#include "mud.h"

/* creation */

ENTITY_DATA *init_entity( void )
{
   ENTITY_DATA *entity;

   entity 			= (ENTITY_DATA *)calloc( 1, sizeof( ENTITY_DATA ) );
   entity->tag			= init_tag();
   entity->inventory		= AllocList();
   for( int x = 0; x < MAX_ENTITY_TYPE; x++ )
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
   int counter = 1;
   entity->script = strdup( (*row)[counter++] );
   string_to_bool_array( (*row)[counter++], entity->type, MAX_ENTITY_TYPE );
   string_to_bool_array( (*row)[counter++], entity->subtype, MAX_ENTITY_SUB_TYPE );
   entity->level = atoi( (*row)[counter++] );
   entity->contained_byID = atoi( (*row)[counter++] );

   entity->ismapped = (bool)atoi( (*row)[counter++] );
   entity->map.min_x = atoi( (*row)[counter++] );
   entity->map.max_x = atoi( (*row)[counter++] );
   entity->map.min_y = atoi( (*row)[counter++] );
   entity->map.max_y = atoi( (*row)[counter++] );
   entity->map.min_z = atoi( (*row)[counter++] );
   entity->map.max_z = atoi( (*row)[counter++] );

   entity->coord.x = atoi( (*row)[counter++] );
   entity->coord.y = atoi( (*row)[counter++] );
   entity->coord.z = atoi( (*row)[counter++] );

   snprintf( entity->look.floor_tile, 5, "%s", (*row)[counter++] );
   snprintf( entity->look.ceiling_tile, 5, "%s", (*row)[counter++] );

   return counter;
}

bool load_entityInventory( ENTITY_DATA *entity )
{
   ENTITY_DATA *invEntity;
   LLIST *row_list;
   MYSQL_ROW row;
   ITERATOR Iter;
   char query[MAX_BUFFER];

   if( !VALID_TAG( entity ) )
   {
      bug( "%s: attempting to load inventory on entity with a bad tag.", __FUNCTION__ );
      return FALSE;
   }

   snprintf( query, MAX_BUFFER, "SELECT entityID FROM `entities` WHERE containedBy=%d;", GET_ID( entity ) );
   row_list = AllocList();
   if( !db_query_list_row( row_list, query ) )
   {
      FreeList( row_list );
      return TRUE;
   }

   AttachIterator( &Iter, row_list );
   while( ( row = (MYSQL_ROW)NextInList( &Iter ) ) != NULL )
   {
      if( ( invEntity = (ENTITY_DATA *)get_entityByID( atoi( row[0] ) ) ) == NULL )
      {
         bug( "%s: database could not load entity #%d for #%d's inventory.", __FUNCTION__, atoi( row[0] ), GET_ID( entity ) );
         continue;
      }
      entity_to_container( invEntity, entity );
   }
   DetachIterator( &Iter );
   FreeList( row_list );
   return TRUE;
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
      if( !quick_query( "INSERT INTO `entities` VALUES( '%d', '%s', '%s', '%s', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%s', '%s' );",
         GET_ID( entity ), entity->script, type, subtype, entity->level, entity->contained_by ? GET_ID( entity->contained_by ) : 0,
         (int)entity->ismapped, entity->map.min_x, entity->map.max_x, entity->map.min_y, entity->map.max_y, entity->map.min_z, entity->map.max_z,
         entity->coord.x, entity->coord.y, entity->coord.z, entity->look.floor_tile, entity->look.ceiling_tile ) )
      {
         bug( "%s: could not add new entity to database.", __FUNCTION__ );
         return 0;
      }
   }
   return TRUE;
}

void free_entity( ENTITY_DATA *entity )
{
   ENTITY_DATA *invEntity;
   ITERATOR Iter;

   free_tag( entity->tag );
   entity->tag = NULL;
   free_state( entity->managing_state );
   entity->managing_state = NULL;
   FREE( entity->script );

   for( int x = 0; x < ENTITY_HASH; x++ )
   {
      clearlist( entity->inventory_qs[x] );
      FreeList( entity->inventory_qs[x] );
      entity->inventory_qs[x] = NULL;
   }

   AttachIterator( &Iter, entity->inventory );
   while( ( invEntity = (ENTITY_DATA *)NextInList( &Iter ) ) != NULL )
      free_entity( invEntity );
   DetachIterator( &Iter );
   FreeList( entity->inventory );
   entity->inventory = NULL;
   FREE( entity );
}

bool delete_entity( ENTITY_DATA *entity )
{
   ENTITY_DATA *invEntity;
   ITERATOR Iter;

   if( !quick_query( "DELETE FROM `entities` WHERE entityID=%d;", GET_ID( entity ) ) )
   {
      bug( "%s: could not delete entity with id %d from db.", __FUNCTION__, GET_ID( entity ) );
      return FALSE;
   }

   AttachIterator( &Iter, entity->inventory );
   while( ( invEntity = (ENTITY_DATA *)NextInList( &Iter ) ) != NULL )
   {
      DetachFromList( invEntity, entity->inventory );
      entity_from_its_container( invEntity );
   }
   DetachIterator( &Iter );

   delete_tag( entity->tag );
   free_entity( entity );
   return TRUE;
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
   load_entityInventory( entity );

   if( entity->contained_byID != 0 && ( container = get_entityByID_ifActive( entity->contained_byID ) ) != NULL )
      entity_to_container( entity, container );
   else if( !entity->type[ENTITY_ROOM] )
      bug( "%s: left entity #%d floating in limbo and it's not a room.", __FUNCTION__, GET_ID( entity ) );

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

/* setters */
bool set_eScript( ENTITY_DATA *entity, const char *script )
{
   FREE( entity->script );
   entity->script = strdup( script );
   if( VALID_TAG( entity ) )
      if( !quick_query( "UPDATE `entities` SET script='%s' WHERE entityID=%d;", entity->script, GET_ID( entity ) ) )
         bug( "%s: could not update database with new script.", __FUNCTION__ );
   return TRUE;
}
bool set_eType( ENTITY_DATA *entity, ENTITY_TYPE type )
{
   if( !entity )
   {
      bug( "%s: entity is NULL.", __FUNCTION__ );
      return FALSE;
   }

   if( entity->type[type] )
      return TRUE;

   entity->type[type] = TRUE;
   if( entity->contained_by )
      AttachToList( entity, entity->contained_by->inventory_qs[type] );

   if( VALID_TAG( entity ) )
   {
      char type_string[256];
      snprintf( type_string, 256, "%s", bool_array_to_string( entity->type, MAX_ENTITY_TYPE ) );
      if( !quick_query( "UPDATE `entities` SET type='%s' WHERE entityID=%d;", type_string, GET_ID( entity ) ) )
         bug( "%s: could not update database with new type.", __FUNCTION__ );
   }
   return TRUE;
}

bool set_eSubType( ENTITY_DATA *entity, ENTITY_SUB_TYPE stype )
{
   if( !entity )
   {
      bug( "%s: entity is NULL.", __FUNCTION__ );
      return FALSE;
   }

   if( entity->subtype[stype] )
      return TRUE;

   entity->subtype[stype] = TRUE;
   if( VALID_TAG( entity ) )
   {
      char stype_string[256];
      snprintf( stype_string, 256, "%s", bool_array_to_string( entity->subtype, MAX_ENTITY_SUB_TYPE ) );
      if( !quick_query( "UPDATE `entities` SET subtype='%s' WHERE entityID=%d;", stype_string, GET_ID( entity ) ) )
         bug( "%s: could not update databse with new subtype.", __FUNCTION__ );
   }
   return TRUE;
}

bool set_eLevel( ENTITY_DATA *entity, int level )
{
   if( !entity )
   {
      bug( "%s: entity is NULL.", __FUNCTION__ );
      return FALSE;
   }

   entity->level = level;
   if( VALID_TAG( entity ) )
      if( !quick_query( "UPDATE `entities` SET level=%d WHERE entityID=%d;", entity->level, GET_ID( entity ) ) )
         bug( "%s: couldnot update database with newlevel.", __FUNCTION__ );
   return TRUE;
}

/* utility */
bool entity_from_its_container( ENTITY_DATA *entity )
{
   ENTITY_DATA *container;

   if( !entity )
   {
      bug( "%s: trying to 'from' a NULL entity.", __FUNCTION__ );
      return FALSE;
   }

   if( ( container = entity->contained_by ) == NULL )
      return TRUE;

   DetachFromList( entity, container->inventory );
   for( int x = 0; x < MAX_ENTITY_TYPE; x++ )
      if( entity->type[x] == TRUE )
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

   if( !entity_from_its_container( entity ) )
      return FALSE;

   if( !container )
   {
      entity->contained_by = NULL;
      entity->contained_byID = 0;
      return TRUE;
   }

   AttachToList( entity, container->inventory );
   for( int x = 0; x < MAX_ENTITY_TYPE; x++ )
      if( entity->type[x] == TRUE )
         AttachToList( entity, container->inventory_qs[x] );
   entity->contained_byID = GET_ID( container );

   return TRUE;
}

bool update_position( ENTITY_DATA *entity )
{
   if( VALID_TAG( entity ) && ListHas( active_entities[GET_ID( entity ) % ENTITY_HASH], entity ) )
      if( !quick_query( "UPDATE `entities` SET containedBy=%d WHERE entityID=%d;", entity->contained_byID, GET_ID( entity ) ) )
         return FALSE;
   return TRUE;
}

bool toggle_eType( ENTITY_DATA *entity, ENTITY_TYPE type )
{
   if( !entity )
   {
      bug( "%s: entity is NULL.", __FUNCTION__ );
      return FALSE;
   }

   if( entity->type[type] )
   {
      entity->type[type] = FALSE;
      if( entity->contained_by )
         DetachFromList( entity, entity->contained_by->inventory_qs[type ] );
   }
   else
   {
      entity->type[type] = TRUE;
      if( entity->contained_by )
         AttachToList( entity, entity->contained_by->inventory_qs[type] );
   }

   if( VALID_TAG( entity ) )
   {
      char type_string[256];
      snprintf( type_string, 256, "%s", bool_array_to_string( entity->type, MAX_ENTITY_TYPE ) );
      if( !quick_query( "UPDATE `entities` SET type='%s' WHERE entityID=%d;", type_string, GET_ID( entity ) ) )
         bug( "%s: could not update database with new type.", __FUNCTION__ );
   }
   return TRUE;
}

bool toggle_eSubType( ENTITY_DATA *entity, ENTITY_SUB_TYPE stype )
{
   if( !entity )
   {
      bug( "%s: entity is NULL.", __FUNCTION__ );
      return FALSE;
   }

   if( entity->subtype[stype] )
      entity->subtype[stype] = FALSE;
   else
      entity->subtype[stype] = TRUE;

   if( VALID_TAG( entity ) )
   {
      char stype_string[256];
      snprintf( stype_string, 256, "%s", bool_array_to_string( entity->subtype, MAX_ENTITY_SUB_TYPE ) );
      if( !quick_query( "UPDATE `entities` SET subtype='%s' WHERE entityID=%d;", stype_string, GET_ID( entity ) ) )
         bug( "%s: could not update databse with new subtype.", __FUNCTION__ );
   }
   return TRUE;
}

