/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */

#pragma pack(2)
#include <powerpc/powerpc_protos.h>
#pragma pack()

#include <string.h>

void *memcpy(void *s1,const void *s2,size_t n)
{
  if (s1 == NULL || s2 == NULL || n == 0)
    return NULL;
  CopyMemPPC((APTR)s2,s1,n); return s1;
}
