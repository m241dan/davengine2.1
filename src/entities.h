#define MAX_ENTITY_HASH 10

extern LLIST *active_entities[MAX_ENTITY_HASH];

typedef enum
{
   ENTITY_MOBILE, ENTITY_ROOM, ENTITY_EXIT, ENTITY_OBJECT,
   MAX_ENTITY_TYPE
} ENTITY_TYPE;

struct entity
{
   ID_TAG *tag;
   SOCKET_STATE *managing_state;
   char *script;
   bool type[MAX_ENTITY_TYPE];
   /* bool subtype[MAX_SUB_ENTITY_TYPE]; */

   ENTITY_DATA *contained_by;
   LLIST *inventory;
   LLIST *inventory_qs[MAX_ENTITY_TYPE];
};

/* creation */

ENTITY_DATA *init_entity( void );
bool new_entity( ENTITY_DATA *entity );

void free_entity( ENTITY_DATA *entity );
bool delete_entity( ENTITY_DATA *entity );


