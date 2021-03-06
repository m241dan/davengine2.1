/* file: list.h
 *
 * Headerfile for a basic double-linked list
 */

#ifndef _LIST_HEADER
#define _LIST_HEADER

typedef struct Cell
{
  struct Cell  *_pNextCell;
  struct Cell  *_pPrevCell;
  void         *_pContent;
  int           _valid;
} CELL;

typedef struct List
{
  CELL  *_pFirstCell;
  CELL  *_pLastCell;
  int    _iterators;
  int    _size;
  int    _valid;
} LLIST;

typedef struct Iterator
{
  LLIST  *_pList;
  CELL  *_pCell;
} ITERATOR;

LLIST *AllocList         ( void );
void  AttachIterator     ( ITERATOR *pIter, LLIST *pList);
void  AttachIteratorEnd  ( ITERATOR *pIter, LLIST *pList);
void *NextInList         ( ITERATOR *pIter );
CELL *NextCellInList     ( ITERATOR *pIter );
void *PrevInList	 ( ITERATOR *pIter );
void *GetFromListIndex   ( LLIST *pList, int index );
void  AttachToList       ( void *pContent, LLIST *pList );
void  AttachToEnd	 ( void *pContent, LLIST *pList );
void  InsertBefore	 ( void *pContent, LLIST *pList, void *bContent );
void  InsertAfter	 ( void *pContent, LLIST *pList, void *aContent );
void  DetachFromList     ( void *pContent, LLIST *pList );
void  DetachIterator     ( ITERATOR *pIter );
void  FreeList           ( LLIST *pList );
int   SizeOfList         ( LLIST *pList );
int   ListHas		 ( LLIST *pList, void *pContent );
#endif
