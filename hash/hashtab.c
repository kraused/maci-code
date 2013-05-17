/*
--------------------------------------------------------------------
By Bob Jenkins, 1996.  hasHashlib_privatescope_htab.c.  Public Domain.

This implements a hash table.
* Keys are unique.  Adding an item fails if the key is already there.
* Keys and items are pointed at, not copied.  If you change the value
  of the key after it is inserted then hfind will not be able to find it.
* The hash table maintains a position that can be set and queried.
* The table length doubles dynamically and never shrinks.  The insert
  that causes table doubling may take a long time.
* The table length splits when the table length equals the number of items
  Comparisons usually take 7 instructions.
  Computing a hash value takes 35+6n instructions for an n-byte key.

  hcreate  - create a hash table
  hdestroy - destroy a hash table
   hcount  - The number of items in the hash table
   hkey    - key at the current position
   hkeyl   - key length at the current position
   hstuff  - stuff at the current position
  hfind    - find an item in the table
   hadd    - insert an item into the table
   hdel    - delete an item from the table
  hstat    - print statistics about the table
   Hashlib_privatescope_hfirst  - position at the first item in the table
   hnext   - move the position to the next item in the table
--------------------------------------------------------------------
*/

/* Added by D.K. @ 18.06.09 */
#include <stdlib.h>
#include <string.h>

#include "standard.h"
#include "lookupa.h"
#include "hashtab.h"
#include "recycle.h"

/* Added by D.K. @ 05.09.10 */
#ifdef HSANITY
/* sanity check -- make sure ipos, apos, and count make sense */
static void  Hashlib_privatescope_hsanity(Hashlib_privatescope_htab *t)
{
  ub4    i, end, counter;
  Hashlib_privatescope_hitem *h;

  /* test that apos makes sense */
  end = (ub4)1<<(t->logsize);
  if (end < t->apos)
    printf("error:  end %ld  apos %ld\n", end, t->apos);

  /* test that ipos is in bucket apos */
  if (t->ipos)
  {
    for (h=t->table[t->apos];  h && h != t->ipos;  h = h->next)
      ;
    if (h != t->ipos)
      printf("error:ipos not in apos, apos is %ld\n", t->apos);
  }

  /* test that t->count is the number of elements in the table */
  counter=0;
  for (counter=0, i=0;  i<end;  ++i)
    for (h=t->table[i];  h;  h=h->next)
      ++counter;
  if (counter != t->count)
    printf("error: counter %ld  t->count %ld\n", counter, t->count);
}
#endif

/*
 * hgrow - Double the size of a hash table.
 * Allocate a new, 2x bigger array,
 * move everything from the old array to the new array,
 * then free the old array.
 */
static void Hashlib_privatescope_hgrow(Hashlib_privatescope_htab  *t)
{
  register ub4     newsize = (ub4)1<<(++t->logsize);
  register ub4     newmask = newsize-1;
  register ub4     i;
  register Hashlib_privatescope_hitem **oldtab = t->table;
  register Hashlib_privatescope_hitem **newtab = (Hashlib_privatescope_hitem **)malloc(newsize*sizeof(Hashlib_privatescope_hitem *));

  /* make sure newtab is cleared */
  for (i=0; i<newsize; ++i) newtab[i] = (Hashlib_privatescope_hitem *)0;
  t->table = newtab;
  t->mask = newmask;

  /* Walk through old table putting entries in new table */
  for (i=newsize>>1; i--;)
  {
    register Hashlib_privatescope_hitem *this, *that, **newplace;
    for (this = oldtab[i]; this;)
    {
      that = this;
      this = this->next;
      newplace = &newtab[(that->hval & newmask)];
      that->next = *newplace;
      *newplace = that;
    }
  }

  /* position the hash table on some existing item */
  Hashlib_privatescope_hfirst(t);

  /* free the old array */
  free((char *)oldtab);

}

/* hcreate - create a hash table initially of size power(2,logsize) */
Hashlib_privatescope_htab *Hashlib_privatescope_hcreate(word  logsize)
{
  ub4 i,len;
  Hashlib_privatescope_htab *t = (Hashlib_privatescope_htab *)malloc(sizeof(Hashlib_privatescope_htab));

  len = ((ub4)1<<logsize);
  t->table = (Hashlib_privatescope_hitem **)malloc(sizeof(Hashlib_privatescope_hitem *)*(ub4)len);
  for (i=0; i<len; ++i) t->table[i] = (Hashlib_privatescope_hitem *)0;
  t->logsize = logsize;
  t->mask = len-1;
  t->count = 0;
  t->apos = (ub4)0;
  t->ipos = (Hashlib_privatescope_hitem *)0;
  t->space = Hashlib_privatescope_remkroot(sizeof(Hashlib_privatescope_hitem));
  t->bcount = 0;
  return t;
}

/* hdestroy - destroy the hash table and free all its memory */
void Hashlib_privatescope_hdestroy(Hashlib_privatescope_htab  *t)
{
  Hashlib_privatescope_refree(t->space);
  free((char *)t->table);
  free((char *)t);
}

/* hcount() is a macro, see hasHashlib_privatescope_htab.h */
/* hkey() is a macro, see hasHashlib_privatescope_htab.h */
/* hkeyl() is a macro, see hasHashlib_privatescope_htab.h */
/* hstuff() is a macro, see hasHashlib_privatescope_htab.h */

/* hfind - find an item with a given key in a hash table */
word   Hashlib_privatescope_hfind( t, key, keyl )
Hashlib_privatescope_htab  *t;     /* table */
ub1   *key;   /* key to find */
ub4    keyl;  /* key length */
{
  Hashlib_privatescope_hitem *h;
  ub4    x = Hashlib_privatescope_lookup(key,keyl,0);
  ub4    y;
  for (h = t->table[y=(x&t->mask)]; h; h = h->next)
  {
    if ((x == h->hval) && 
        (keyl == h->keyl) && 
        !memcmp(key, h->key, keyl))
    {
      t->apos = y;
      t->ipos = h;
      return TRUE;
    }
  }
  return FALSE;
}

/*
 * hadd - add an item to a hash table.
 * return FALSE if the key is already there, otherwise TRUE.
 */
word Hashlib_privatescope_hadd( t, key, keyl, stuff)
Hashlib_privatescope_htab  *t;      /* table */
ub1   *key;    /* key to add to hash table */
ub4    keyl;   /* key length */
void  *stuff;  /* stuff to associate with this key */
{
  register Hashlib_privatescope_hitem  *h,**hp;
  register ub4     y, x = Hashlib_privatescope_lookup(key,keyl,0);

  /* make sure the key is not already there */
  for (h = t->table[(y=(x&t->mask))]; h; h = h->next)
  {
    if ((x == h->hval) && 
        (keyl == h->keyl) && 
        !memcmp(key, h->key, keyl))
    {
      t->apos = y;
      t->ipos = h;
      return FALSE;
    }
  }

  /* find space for a new item */
  h = (Hashlib_privatescope_hitem *)Hashlib_privatescope_renew(t->space);

  /* make the hash table bigger if it is getting full */
  if (++t->count > (ub4)1<<(t->logsize))
  {
    Hashlib_privatescope_hgrow(t);
    y = (x&t->mask);
  }

  /* add the new key to the table */
  h->key   = key;
  h->keyl  = keyl;
  h->stuff = stuff;
  h->hval  = x;
  hp = &t->table[y];
  h->next = *hp;
  *hp = h;
  t->ipos = h;
  t->apos = y;

#ifdef HSANITY
  Hashlib_privatescope_hsanity(t);
#endif  /* HSANITY */

  return TRUE;
}

/* hdel - delete the item at the current position */
word  Hashlib_privatescope_hdel(t)
Hashlib_privatescope_htab *t;      /* the hash table */
{
  Hashlib_privatescope_hitem  *h;    /* item being deleted */
  Hashlib_privatescope_hitem **ip;   /* a counter */

  /* check for item not existing */
  if (!(h = t->ipos)) return FALSE;

  /* remove item from its list */
  for (ip = &t->table[t->apos]; *ip != h; ip = &(*ip)->next)
    ;
  *ip = (*ip)->next;
  --(t->count);

  /* adjust position to something that exists */
  if (!(t->ipos = h->next)) Hashlib_privatescope_hnbucket(t);

  /* Hashlib_privatescope_recycle the deleted Hashlib_privatescope_hitem node */
  Hashlib_privatescope_redel(t->space, h);

#ifdef HSANITY
  Hashlib_privatescope_hsanity(t);
#endif  /* HSANITY */

  return TRUE;
}

/* Hashlib_privatescope_hfirst - position on the first element in the table */
word Hashlib_privatescope_hfirst(t)
Hashlib_privatescope_htab  *t;    /* the hash table */
{
  t->apos = t->mask;
  (void)Hashlib_privatescope_hnbucket(t);
  return (t->ipos != (Hashlib_privatescope_hitem *)0);
}

/* hnext() is a macro, see hasHashlib_privatescope_htab.h */

/*
 * hnbucket - Move position to the first item in the next bucket.
 * Return TRUE if we did not wrap around to the beginning of the table
 */
word Hashlib_privatescope_hnbucket(t)
Hashlib_privatescope_htab *t;
{
  ub4  oldapos = t->apos;
  ub4  end = (ub4)1<<(t->logsize);
  ub4  i;

  /* see if the element can be found without wrapping around */
  for (i=oldapos+1; i<end; ++i)
  {
    if (t->table[i&t->mask])
    {
      t->apos = i;
      t->ipos = t->table[i];
      return TRUE;
    }
  }

  /* must have to wrap around to find the last element */
  for (i=0; i<=oldapos; ++i)
  {
    if (t->table[i])
    {
      t->apos = i;
      t->ipos = t->table[i];
      return FALSE;
    }
  }

  return FALSE;
}

void Hashlib_privatescope_hstat(t)
Hashlib_privatescope_htab  *t;
{
  ub4     i,j;
  double  total = 0.0;
  Hashlib_privatescope_hitem  *h;
  Hashlib_privatescope_hitem  *walk, *walk2, *stat = (Hashlib_privatescope_hitem *)0;

  /* in stat, keyl will store length of list, hval the number of buckets */
  for (i=0; i<=t->mask; ++i)
  {
    for (h=t->table[i], j=0; h; ++j, h=h->next)
      ;
    for (walk=stat; walk && (walk->keyl != j); walk=walk->next)
      ;
    if (walk)
    {
      ++(walk->hval);
    }
    else
    {
      walk = (Hashlib_privatescope_hitem *)Hashlib_privatescope_renew(t->space);
      walk->keyl = j;
      walk->hval = 1;
      if (!stat || stat->keyl > j) {walk->next=stat; stat=walk;}
      else
      {
        for (walk2=stat;
             walk2->next && (walk2->next->keyl<j);
             walk2=walk2->next)
          ;
        walk->next = walk2->next;
        walk2->next = walk;
      }
    }
  }

  /* figure out average list length for existing elements */
  for (walk=stat; walk; walk=walk->next)
  {
    total+=(double)walk->hval*(double)walk->keyl*(double)walk->keyl;
  }
  if (t->count) total /= (double)t->count;
  else          total  = (double)0;

  /* print statistics */
  printf("\n");
  for (walk=stat; walk; walk=walk->next)
  {
    printf("items %ld:  %ld buckets\n", walk->keyl, walk->hval);
  }
  printf("\nbuckets: %ld  items: %ld  existing: %g\n\n",
         ((ub4)1<<t->logsize), t->count, total);

  /* clean up */
  while (stat)
  {
    walk = stat->next;
    Hashlib_privatescope_redel(t->space, stat);
    stat = walk;
  }
}


