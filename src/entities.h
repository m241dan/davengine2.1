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

   ENTITY_DATA *contained_by;
   int contained_byID;
   LLIST *inventory;
   LLIST *inventory_qs[MAX_ENTITY_TYPE];
};

/* creation */

ENTITY_DATA *init_entity( void );
ENTITY_DATA *load_entityByID( int id );
int db_load_entity( ENTITY_DATA *entity, MYSQL_ROW *row );
bool new_entity( ENTITY_DATA *entity );

void free_entity( ENTITY_DATA *entity );
bool delete_entity( ENTITY_DATA *entity );

/* getters */
ENTITY_DATA *get_entityByID( int id );
ENTITY_DATA *get_entityByID_ifActive( int id );
const char *entity_getScript( ENTITY_DATA *entity );

/* setters */
bool entity_setScript( ENTITY_DATA *entity, const char *script );
bool entity_setType( ENTITY_DATA *entity, ENTITY_TYPE type );

/* utility */
bool entity_from_its_container( ENTITY_DATA *entity );
bool entity_to_container( ENTITY_DATA *entity, ENTITY_DATA *container );
bool update_position( ENTITY_DATA *entity );
