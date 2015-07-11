struct socket_state
{
   D_SOCKET *socket;
   union control_types {
      ACCOUNT_DATA *account;
      NANNY_DATA *nanny;
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
void set_state_as_nanny( SOCKET_STATE *state, NANNY_DATA *nanny );
void set_state_as_entity( SOCKET_STATE *state, ENTITY_DATA *entity );

/* getters */


/* utility */
