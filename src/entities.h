#define ENTITY_HASH 10

extern LLIST *active_entities[ENTITY_HASH];

typedef enum
{
   ENTITY_MOBILE, ENTITY_ROOM, ENTITY_EXIT, ENTITY_OBJECT,
   MAX_ENTITY_TYPE
} ENTITY_TYPE;

typedef enum
{
   ENTITY_SUB_NONE,
   MAX_ENTITY_SUB_TYPE
} ENTITY_SUB_TYPE;

struct entity
{
   ID_TAG *tag;
   SOCKET_STATE *managing_state;
   char *script;
   bool type[MAX_ENTITY_TYPE];
   bool subtype[MAX_ENTITY_SUB_TYPE];

   bool is_mapped;
   int coord_x;
   int coord_y;
   int coord_z;

   ENTITY_DATA *contained_by;
   long long contained_byID;
   LLIST *inventory;
   LLIST *inventory_qs[MAX_ENTITY_TYPE];
};

/* creation */

ENTITY_DATA *init_entity( void );
ENTITY_DATA *load_entityByID( long long id );
int db_load_entity( ENTITY_DATA *entity, MYSQL_ROW *row );
bool load_entityInventory( ENTITY_DATA *entity );
bool new_entity( ENTITY_DATA *entity );

void free_entity( ENTITY_DATA *entity );
bool delete_entity( ENTITY_DATA *entity );

/* getters */
ENTITY_DATA *get_entityByID( long long id );
ENTITY_DATA *get_entityByID_ifActive( long long id );

/* setters */
bool entity_setScript( ENTITY_DATA *entity, const char *script );
bool entity_setType( ENTITY_DATA *entity, ENTITY_TYPE type );
bool entity_setSubType( ENTITY_DATA *entity, ENTITY_SUB_TYPE stype );

/* utility */
bool entity_from_its_container( ENTITY_DATA *entity );
bool entity_to_container( ENTITY_DATA *entity, ENTITY_DATA *container );
bool update_position( ENTITY_DATA *entity );
