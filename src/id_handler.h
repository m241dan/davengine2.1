/* id-handlers.h written by Davenge */

struct id_tag
{
   unsigned char type;
   int id;
   unsigned char can_recycle;
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

int get_top_id( int type );
int get_new_id( int type );
int get_potential_id( int type );
int can_tag_be_recycled( int type );
ID_TAG *copy_tag( ID_TAG *tag );

