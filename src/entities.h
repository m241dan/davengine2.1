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
   LLIST *inventory;
   LLIST *inventory_qs[MAX_ENTITY_TYPE];
};

/* creation */

ENTITY_DATA *init_entity( void );
ENTITY_DATA *load_entityByID( int id );
ENTITY_DATA *load_entityByName( const char *name );
int db_load_entity( ENTITY_DATA *entity );
bool new_entity( ENTITY_DATA *entity );

void free_entity( ENTITY_DATA *entity );
bool delete_entity( ENTITY_DATA *entity );

/* getters */
ENTITY_DATA *get_entityByID( int id );
ENTITY_DATA *get_entityByID_ifActive( int id );
const char *get_script( ENTITY_DATA *entity );

/* setters */
bool set_script( ENTITY_DATA *entity, const char *script );
