/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>

#pragma pack(2)
#include <proto/dos.h>
#include <dos/dos.h>
#pragma pack()

extern void __seterrno(void);
extern char *__amigapath(const char *path);

int _symlink (const char *path1, const char *path2)
{
  int result = -1;
  LONG status;
	
  if (path1 == NULL || path2 == NULL)
    {
    errno = EFAULT;
    return -1;
    }

  if ((path1=__amigapath(path1))!=NULL)
    {
    if ((path2=__amigapath(path2))!=NULL)
      {
      status = MakeLink((STRPTR)path2,(LONG)path1,LINK_SOFT);
      if (status == DOSFALSE)
        {
        __seterrno();
        return -1;
        }
      }
    }
return 0;
}




