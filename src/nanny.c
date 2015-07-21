/* nanny.c written by Davenge */

#include "mud.h"

/* creation */

NANNY_DATA *init_nanny( void )
{
   NANNY_DATA *nanny;
   nanny			= malloc( sizeof( NANNY_DATA ) );
   nanny->tag			= init_tag();
   nanny->managing_state	= NULL;
   nanny->lua_path		= NULL;
   nanny->state			= 0;
   nanny->can_back		= FALSE;
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
   AttachToList( nanny, active_nannys );
   lua_pushobj( lua_handle, nanny, NANNY_DATA );
   if( ( ret = lua_pcall( lua_handle, 1, LUA_MULTRET, 0 ) ) )
   {
      bug( "%s: ret %d: path %s\r\n - error message: %s.", __FUNCTION__, ret, nanny->lua_path, lua_tostring( lua_handle, -1 ) );
      return FALSE;
   }
   return TRUE;
}
/* deletion */

void free_nanny( NANNY_DATA *nanny )
{
   delete_tag( nanny->tag );
   if( nanny->state )
      free_state( nanny->managing_state );
   FREE( nanny->lua_path );
   FREE( nanny );
}

/* getters */
NANNY_DATA *get_nannyByID_ifActive( int id )
{
   NANNY_DATA *nanny;
   ITERATOR Iter;

   AttachToList( &Iter, active_nannys );
   while( ( nanny = (NANNY_DATA *)NextInList( &Iter ) ) != NULL )
      if( GET_ID( nanny ) == id )
         break;
   DetachIterator( &Iter );

   return nanny;
}

/* utility */
bool set_nPath( NANNY_DATA *nanny, const char *path )
{
   FREE( nanny->lua_path );
   nanny->lua_path = new_string( path );
   return TRUE;
}

bool set_nState( NANNY_DATA *nanny, int state )
{
   int ret, top = lua_gettop( lua_handle );
   nanny->state = state;

   prep_stack( nanny->lua_path, "onStateChange" );
   lua_pushobj( lua_handle, nanny, NANNY_DATA );
   if( ( ret = lua_pcall( lua_handle, 1, LUA_MULTRET, 0 ) ) )
      bug( "%s: ret %d: path %s\r\n - error message: %s.", __FUNCTION__, ret, nanny->lua_path, lua_tostring( lua_handle, -1 ) );
   lua_settop( lua_handle, top );

   return TRUE;
}

bool state_nNext( NANNY_DATA *nanny )
{
   set_nState( nanny, nanny->state + 1 );
   return TRUE;
}

bool state_nPrev( NANNY_DATA *nanny )
{
   set_nState( nanny, nanny->state - 1 );
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


