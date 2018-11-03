/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#pragma pack(2)
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <proto/exec.h>
#pragma pack()

extern void __seterrno(void);
extern char *__amigapath(const char *path);
extern int __mystat(struct stat *buf, struct FileInfoBlock *fib);

int _stat (const char *file, struct stat *st)
{
APTR oldwin,*wptr=&((struct Process *)FindTask(NULL))->pr_WindowPtr;
  struct FileInfoBlock *fib;
  BPTR lock;
  int ret=-1;

  if((file=__amigapath(file))==NULL)
    return -1;

  /* avoid possible dos-requesters ;-( */

  oldwin=*wptr; *wptr=(APTR)ret;

  if((lock=Lock((STRPTR)file,SHARED_LOCK))!=0)
  {
    if((fib=(struct FileInfoBlock *)AllocDosObject(DOS_FIB,NULL))!=NULL)
    { if(Examine(lock,fib)!=DOSFALSE)
        ret=__mystat(st,fib);
      else
        __seterrno();
      FreeDosObject(DOS_FIB,fib);
    }
    else
      __seterrno();
    UnLock(lock);
  }
  else
    __seterrno();

  *wptr=oldwin;

  return ret;
}
