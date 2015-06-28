/* id-handlers.h written by Davenge */

typedef enum
{
   TAG_INT, TAG_STRING, TAG_UNSET, ACCOUNT_TAG = 0, NANNY_TAG, GLOBAL_TAG, MAX_TAG_TYPE
} TAG_TYPE;

#define GET_ID( obj )	(obj)->tag->id
#define GET_TYPE( obj )	(obj)->tag->type
#define SET_TYPE( obj, val ) (obj)->tag->type = (val)

struct id_tag
{
   TAG_TYPE type;
   int id;
   bool can_recycle;
   char *created_by;
   char *created_on;
   char *modified_by;
   char *modified_on;
};

ID_TAG *init_tag( void );
int clear_tag( ID_TAG *tag );
int free_tag( ID_TAG *tag );
int delete_tag( ID_TAG *tag );
int new_tag( ID_TAG *tag, const char *creator );
int db_load_tag( ID_TAG *tag, MYSQL_ROW *row );
int update_tag( ID_TAG *tag, const char *effector, ... );

int get_top_id( TAG_TYPE type );
int get_new_id( TAG_TYPE type );
int get_potential_id( TAG_TYPE type );
bool can_tag_be_recycled( TAG_TYPE type );
int get_recycled_id( TAG_TYPE type );
ID_TAG *copy_tag( ID_TAG *tag );

