/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#pragma pack(2)
#include <proto/dos.h>
#pragma pack()

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

extern void __seterrno();

char *getcwd(char *buf,size_t size)
{
  if (buf!=NULL || (buf=(char *)malloc(size))!=NULL)
  {
    if (GetCurrentDirName(buf,(ULONG)size)==DOSFALSE)
    {
       __seterrno(); buf=NULL;
    }
  }
  else
    errno=ENOMEM;
  return buf;
}
