/* header file for buffers library */

typedef struct mud_buffer  D_BUFFER;
typedef enum
{
   TOP_FAVOR, BOT_FAVOR, MID_FAVOR, MAX_FAVOR
} BUFFER_FAVOR;

struct mud_buffer
{
   int           width;
   LLIST        *lines;
   BUFFER_FAVOR  favor;
   void         *_bucket;
   int           _reached;
};

#define create_buffer( buf, width, input )                      \
{                                                               \
   (buf) = new_buffer( (width) );                               \
   if( !parse_into_buffer( (buf), (input) ) )                   \
      bug( "%s: could not create a buffer", __FUNCTION__ );     \
}

/* setters */
int              set_buffer_width       ( D_BUFFER *buf, int width );

/* utility */
int              parse_into_buffer      ( D_BUFFER *buf, char *input );
int              set_favor              ( D_BUFFER *buf, BUFFER_FAVOR favor );
char            *buffer_to_string       ( D_BUFFER *buf );
char            *buffers_to_string      ( D_BUFFER *buffers[], int size );
int              get_buffer_length      ( D_BUFFER *buf );
int              clear_buffer           ( D_BUFFER *buf );

