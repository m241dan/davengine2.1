#define ENTITY_HASH	10
#define MAX_TILE	5

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

struct coord_set
{
   int x;
   int y;
   int z;
};

struct map_data
{
   int min_x;
   int max_x;
   int min_y;
   int max_y;
   int min_z;
   int max_z;
};

struct entity_look
{
   char floor_tile[MAX_TILE];
   char ceiling_tile[MAX_TILE];
};

struct entity
{
   ID_TAG *tag;
   SOCKET_STATE *managing_state;
   char *script;
   bool type[MAX_ENTITY_TYPE];
   bool subtype[MAX_ENTITY_SUB_TYPE];
   int level;

   bool ismapped;
   MAP_DATA map;
   COORD_SET coord;
   ENTITY_LOOK look;

   ENTITY_DATA *contained_by;
   int contained_byID;
   LLIST *inventory;
   LLIST *inventory_qs[MAX_ENTITY_TYPE];
};

/* creation */

ENTITY_DATA *init_entity( void );
ENTITY_DATA *load_entityByID( int id );
int db_load_entity( ENTITY_DATA *entity, MYSQL_ROW *row );
bool load_entityInventory( ENTITY_DATA *entity );
bool new_entity( ENTITY_DATA *entity );

void free_entity( ENTITY_DATA *entity );
bool delete_entity( ENTITY_DATA *entity );

/* getters */
ENTITY_DATA *get_entityByID( int id );
ENTITY_DATA *get_entityByID_ifActive( int id );

/* setters */
bool set_eScript( ENTITY_DATA *entity, const char *script );
bool set_eType( ENTITY_DATA *entity, ENTITY_TYPE type );
bool set_eSubType( ENTITY_DATA *entity, ENTITY_SUB_TYPE stype );
bool set_eLevel( ENTITY_DATA *entity, int level );

/* utility */
bool entity_from_its_container( ENTITY_DATA *entity );
bool entity_to_container( ENTITY_DATA *entity, ENTITY_DATA *container );
bool update_position( ENTITY_DATA *entity );
bool toggle_eType( ENTITY_DATA *entity, ENTITY_TYPE type );
bool toggle_eSubType( ENTITY_DATA *entity, ENTITY_SUB_TYPE stype );
