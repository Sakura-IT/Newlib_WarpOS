/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#pragma pack(push,2)
#include <powerpc/powerpc_protos.h>
#pragma pack(pop)

extern void *memamiga(size_t align, size_t nbytes);

void *_malloc_r(struct _reent *re, size_t size)
{
  return malloc(size);
}


void *_realloc_r(struct _reent *re, void *ptr, size_t size)
{
  return realloc(ptr, size);
}


void *calloc(size_t num, size_t size)
{
  void * ptr;
  ptr = memamiga(32L, num*size);
  if (ptr)
    memset (ptr, 0, num*size);
  return ptr;
}


void *_calloc_r(struct _reent *re, size_t num, size_t size)
{
  return calloc(num, size);
}


void *_memalign_r(struct _reent *re, size_t align, size_t nbytes)
{
  return memalign(align, nbytes);
}


void *malloc(size_t size)
{
  return memamiga(32L, size);
}


void *memalign (size_t align, size_t nbytes)
{
  return memamiga(align, nbytes);
}


void _free_r(struct _reent *re, void *ptr)
{
  return free(ptr);
}
