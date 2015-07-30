#include "mud.h"

/* creation */
SOCKET_STATE *init_state( void )
{
   SOCKET_STATE *state;

   state 			= (SOCKET_STATE *)calloc( 1, sizeof( SOCKET_STATE ) );
   state->top_outbuf		= 0;
   state->outbuf[0]		= new_buffer( 70 );
   for( int x = 1; x < OUT_BUFS; x++ )
      state->outbuf[x] = new_buffer( 0 );
   state->control.account 	= NULL;
   state->socket 		= NULL;
   state->type 			= TAG_UNSET;
   return state;
}

void free_state( SOCKET_STATE *state )
{
   if( state->socket )
      socket_delState( state->socket, state );
   state->control.account = NULL;
   FREE( state );
}
/* setters */

void set_state_control( SOCKET_STATE *state, const char *file )
{
   state->control_file = new_string( file );
}

void set_state_as_account( SOCKET_STATE *state, ACCOUNT_DATA *account )
{
   account->managing_state = state;
   state->control.account = account;
   state->type = ACCOUNT_TAG;
}

void set_state_as_entity( SOCKET_STATE *state, ENTITY_DATA *entity )
{
   entity->managing_state = state;
   state->control.entity = entity;
   state->type = ENTITY_TAG;
}
/* getters */

/* utility */
