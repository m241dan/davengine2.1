/* file: list.c
 *
 * The implementation of a basic double-linked list
 */

#include <stdlib.h>

#include "list.h"

/* local procedures */
void  FreeCell        ( CELL *pCell, LLIST *pList );
CELL *AllocCell       ( void );
void  InvalidateCell  ( CELL *pCell );

LLIST *AllocList()
{
  LLIST *pList;

  pList = malloc(sizeof(*pList));
  pList->_pFirstCell = NULL;
  pList->_pLastCell = NULL;
  pList->_iterators = 0;
  pList->_valid = 1;
  pList->_size = 0;

  return pList;
}

void AttachIterator(ITERATOR *pIter, LLIST *pList)
{
  pIter->_pList = pList;

  if (pList != NULL)
  {
    pList->_iterators++;
    pIter->_pCell = pList->_pFirstCell;
  }
  else
    pIter->_pCell = NULL;
}

void AttachIteratorEnd( ITERATOR *pIter, LLIST *pList )
{
   pIter->_pList = pList;

   if( pList != NULL )
   {
      pList->_iterators++;
      pIter->_pCell = pList->_pFirstCell;
   }
   else
      pIter->_pCell = NULL;
}

CELL *AllocCell()
{
  CELL *pCell;

  pCell = malloc(sizeof(*pCell));
  pCell->_pNextCell = NULL;
  pCell->_pPrevCell = NULL;
  pCell->_pContent = NULL;
  pCell->_valid = 1;

  return pCell;
}

void AttachToList(void *pContent, LLIST *pList)
{
  CELL *pCell;
  int found = 0;

  for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pCell->_pNextCell)
  {
    if (!pCell->_valid)
      continue;

    if (pCell->_pContent == pContent)
    {
      found = 1;
      break;
    }
  }

  /* do not attach to list if already here */
  if (found)
    return;

  pCell = AllocCell();
  pCell->_pContent = pContent;
  pCell->_pNextCell = pList->_pFirstCell;

  if (pList->_pFirstCell != NULL)
    pList->_pFirstCell->_pPrevCell = pCell;
  if (pList->_pLastCell == NULL)
    pList->_pLastCell = pCell;

  pList->_pFirstCell = pCell;

  pList->_size++;
}

void AttachToEnd( void *pContent, LLIST *pList )
{
   CELL *pCell;

   for( pCell = pList->_pLastCell; pCell != NULL; pCell = pCell->_pPrevCell )
   {
      if( !pCell->_valid )
         continue;

      if( pCell->_pContent == pContent )
         return;
   }

   pCell = AllocCell();
   pCell->_pContent = pContent;
   pCell->_pPrevCell = pList->_pLastCell;

   if( pList->_pLastCell != NULL )
      pList->_pLastCell->_pNextCell = pCell;
   if( pList->_pFirstCell == NULL )
      pList->_pFirstCell = pCell;

   pList->_pLastCell = pCell;
   pList->_size++;
}

void InsertBefore( void *pContent, LLIST *pList, void *bContent )
{
   CELL *pCell, *bCell;

   for( pCell = pList->_pFirstCell; pCell; pCell = pCell->_pNextCell )
   {
      if( !pCell->_valid )
         continue;

      if( pCell->_pContent == pContent )
         return;
   }

   for( bCell = pList->_pFirstCell; bCell; bCell = bCell->_pNextCell )
   {
      if( !bCell->_valid )
         continue;
      if( bCell->_pContent == bContent )
         break;
   }

   if( !bCell )
      return;

   pCell = AllocCell();
   pCell->_pContent = pContent;
   pCell->_pNextCell = bCell;
   if( bCell->_pPrevCell )
   {
      pCell->_pPrevCell = bCell->_pPrevCell;
      pCell->_pPrevCell->_pNextCell = pCell;
   }
   else if( bCell == pList->_pFirstCell )
      pList->_pFirstCell = pCell;

   bCell->_pPrevCell = pCell;
   pList->_size++;
}

void InsertAfter( void *pContent, LLIST *pList, void *aContent )
{
   CELL *pCell, *aCell;

   for( pCell = pList->_pFirstCell; pCell; pCell = pCell->_pNextCell )
   {
      if( !pCell->_valid )
         continue;
      if( pCell->_pContent == pContent )
         return;
   }

   for( aCell = pList->_pFirstCell; aCell; aCell = aCell->_pNextCell )
   {
      if( !aCell->_valid )
         continue;
      if( aCell->_pContent == aContent )
         break;
   }

   if( !aCell )
      return;

   pCell = AllocCell();
   pCell->_pContent = pContent;
   pCell->_pPrevCell = aCell;
   if( aCell->_pNextCell )
   {
      pCell->_pNextCell = aCell->_pNextCell;
      pCell->_pNextCell->_pPrevCell = pCell;
   }
   else if( aCell == pList->_pLastCell )
      pList->_pLastCell = pCell;

   aCell->_pNextCell = pCell;
   pList->_size++;
}


void DetachFromList(void *pContent, LLIST *pList)
{
  CELL *pCell;

  for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pCell->_pNextCell)
  {
    if (pCell->_pContent == pContent)
    {
      if (pList->_iterators > 0)
        InvalidateCell(pCell);
      else
        FreeCell(pCell, pList);
      pList->_size--;
      return;
    }
  }
}

void DetachIterator(ITERATOR *pIter)
{
  LLIST *pList = pIter->_pList;

  if (pList != NULL)
  {
    pList->_iterators--;

    /* if this is the only iterator, free all invalid cells */
    if (pList->_iterators <= 0)
    {
      CELL *pCell, *pNextCell;

      for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pNextCell)
      {
        pNextCell = pCell->_pNextCell;

        if (!pCell->_valid)
          FreeCell(pCell, pList);
      }

      if (!pList->_valid)
        FreeList(pList);
    }
  }
}

void FreeList(LLIST *pList)
{
  CELL *pCell, *pNextCell;

  /* if we have any unfinished iterators, wait for later */
  if (pList->_iterators > 0)
  {
    pList->_valid = 0;
    return;
  }

  for (pCell = pList->_pFirstCell; pCell != NULL; pCell = pNextCell)
  {
    pNextCell = pCell->_pNextCell;

    FreeCell(pCell, pList);
  }

  free(pList);
}

void FreeCell(CELL *pCell, LLIST *pList)
{
  if (pList->_pFirstCell == pCell)
    pList->_pFirstCell = pCell->_pNextCell;

  if (pList->_pLastCell == pCell)
    pList->_pLastCell = pCell->_pPrevCell;

  if (pCell->_pPrevCell != NULL)
    pCell->_pPrevCell->_pNextCell = pCell->_pNextCell;

  if (pCell->_pNextCell != NULL) 
    pCell->_pNextCell->_pPrevCell = pCell->_pPrevCell;

  free(pCell);
}

void InvalidateCell(CELL *pCell)
{
  pCell->_valid = 0;
}

void *NextInList(ITERATOR *pIter)
{
  void *pContent = NULL;

  /* skip invalid cells */
  while (pIter->_pCell != NULL && !pIter->_pCell->_valid)
  {
    pIter->_pCell = pIter->_pCell->_pNextCell;
  }

  if (pIter->_pCell != NULL)
  {
    pContent = pIter->_pCell->_pContent;
    pIter->_pCell = pIter->_pCell->_pNextCell;
  }

  return pContent;
}
CELL *NextCellInList( ITERATOR *pIter )
{
   CELL *pCell = NULL;
   while( pIter->_pCell != NULL && !pIter->_pCell->_valid )
   {
      pIter->_pCell = pIter->_pCell->_pNextCell;
   }

   if( pIter->_pCell != NULL )
   {
      pCell = pIter->_pCell;
      pIter->_pCell = pIter->_pCell->_pNextCell;
   }
   return pCell;
}

void *PrevInList( ITERATOR *pIter )
{
   void *pContent = NULL;

   while( pIter->_pCell != NULL && !pIter->_pCell->_valid )
      pIter->_pCell = pIter->_pCell->_pPrevCell;

   if( pIter->_pCell != NULL )
   {
      pContent = pIter->_pCell->_pContent;
      pIter->_pCell = pIter->_pCell->_pPrevCell;
   }
   return pContent;
}
/* first cell is index 1, lua style */
void *GetFromListIndex( LLIST *pList, int index )
{
   void *content;
   ITERATOR Iter;
   int counter = 0;

   AttachIterator( &Iter, pList );
   while( ( content = NextInList( &Iter ) ) != NULL )
   {
      if( ++counter == index )
         break;
   }
   DetachIterator( &Iter );
   return content;
}

int SizeOfList(LLIST *pList)
{
  return pList->_size;
}

int ListHas( LLIST *pList, void *pContent )
{
   void *cContent;
   ITERATOR Iter;

   AttachIterator( &Iter, pList );
   while( ( cContent = NextInList( &Iter ) ) != NULL )
      if( cContent == pContent )
         break;
   DetachIterator( &Iter );
   if( cContent )
      return 1;
   return 0;
}
