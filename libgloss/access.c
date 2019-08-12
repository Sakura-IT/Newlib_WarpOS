/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#pragma pack(push,2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack(pop)

extern char *__amigapath(const char *path);
extern void __seterrno(void);

int access (const char *name,int mode)
{ 
  BPTR lock;

  if((name=__amigapath(name))==NULL)
    return -1;

  lock=Lock((char *)name,mode&(O_WRONLY|O_RDWR)?ACCESS_WRITE:ACCESS_READ);
  if (lock!=0l)
    { 
    UnLock(lock);
    return 0;
    }
  else  
    {
    __seterrno();
    return -1;
    }
}
