struct socket_state
{
   D_SOCKET *socket;
   int top_outbuf;
   D_BUFFER *outbuf[OUT_BUFS];

   union control_types {
      ACCOUNT_DATA *account;
      ENTITY_DATA *entity;
   } control;
   sh_int type;

   char *control_file;
};

/* creation */
SOCKET_STATE *init_state( void );
void free_state( SOCKET_STATE *state );

/* setters */
void set_state_control( SOCKET_STATE *state, const char *file );
void set_state_as_account( SOCKET_STATE *state, ACCOUNT_DATA *account );
void set_state_as_entity( SOCKET_STATE *state, ENTITY_DATA *entity );

/* getters */


/* utility */
