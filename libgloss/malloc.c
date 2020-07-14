/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#pragma pack(push,2)
#include <powerpc/powerpc_protos.h>
#pragma pack(pop)

void *memamiga(size_t align, size_t nbytes)
{
  void* ptr;
  ptr = AllocVecPPC(nbytes, MEMF_ANY, align);
  return ptr;
}


void free(void *ptr)
{
  if (ptr != NULL)
  {   
    FreeVecPPC(ptr);
  }
}













