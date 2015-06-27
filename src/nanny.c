/* nanny.c written by Davenge */

#include "mud.h"

/* creation */

NANNY_DATA *init_nanny( void )
{
   NANNY_DATA *nanny;
   nanny		= malloc( sizeof( NANNY_DATA ) );
   nanny->tag		= init_tag();
   nanny->socket	= NULL;
   nanny->lua_path	= NULL;
   nanny->state		= 0;
   nanny->can_back	= FALSE;
   return nanny;
}

bool load_nanny( NANNY_DATA *nanny ) /* nanny must have a set path, then it will load any variables it needs ahead of time based on its lua script */
{
   int ret;

   SET_TYPE( nanny, NANNY_TAG );
   if( !new_tag( nanny->tag, "system" ) )
      return FALSE;
   if( !prep_stack( nanny->lua_path, "onNannyLoad" ) )
   {
      bug( "%s: could not prep stack function call.", __FUNCTION__ );
      return FALSE;
   }
   push_nanny( entity, lua_handle );
   if( ( ret = lua_pcall( lua_handle, 1, LUA_MULTRET, 0 ) ) )
   {
      bug( "%s: ret %d: path %s\r\n - error message: %s.", __FUNCTION__, ret, nanny->lua_path, lua_tostring( lua_hnadle, -1 ) );
      return FALSE;
   }
   return TRUE;
}
/* deletion */

void free_nanny( NANNY_DATA *nanny )
{
   delete_tag( nanny->tag );
   nanny->socket = NULL;
   FREE( nanny->lua_path );
   FREE( nanny );
}

/* utility */
bool set_nPath( NANNY_DATA *nanny, const char *path )
{
   FREE( nanny->lua_path );
   nanny->lua_path = new_string( path );
   return TRUE;
}

bool set_nSocket( NANNY_DATA *nanny, D_SOCKET *socket )
{
   if( nanny->socket )
      nanny->socket->nanny = NULL;
   nanny->socket = socket;
   nanny->socket->nanny = nanny;
   return TRUE;
}

bool set_nState( NANNY_DATA *nanny, int state )
{
   nanny->state = state;
   /* maybe put a state change lua call in here */
   return TRUE;
}

bool nanny_stateNext( NANNY_DATA *nanny )
{
   nanny->state++;
   /* maybe put a state change lua call in here */
   return TRUE;
}

bool nanny_statePrev( NANNY_DATA *nanny )
{
   nanny->state--;
   /* maybe put a state change lua call in here */
   return TRUE;
}

bool onNannyInterp( NANNY_DATA *nanny, const char *input )
{
   /* these are lua calls */
   return FALSE;
}
bool onNannyPrompt( NANNY_DATA *nanny )
{
   /* these are lua calls */
   return FALSE;
}

