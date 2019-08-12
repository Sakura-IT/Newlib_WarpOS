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

void *realloc(void *ptr, size_t size)
{
  if (!ptr)
    {
    return malloc(size);
    }

  if (!size)
    {
    free(ptr);
    return NULL;
    }

  LONG *a = NULL;
  a = malloc(size);
  if (!a)
    return a;
  CopyMemPPC (ptr, a, size);
  free(ptr);
  return a;
}
