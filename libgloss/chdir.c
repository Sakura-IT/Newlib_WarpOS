/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <stdlib.h>

#pragma pack(push,2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack(pop)

extern void __seterrno(void);
extern char *__amigapath(const char *path);

static BPTR oldlock;
static int havelock=0;

int chdir(const char *path)
{
  BPTR fl;
  int ret;
  
  if(path == NULL)
	{
	errno=EFAULT;
	return -1;
	}
  
  STRPTR cpath = (STRPTR)__amigapath(path);
  
  if ((fl=Lock((STRPTR)cpath,SHARED_LOCK)))
  {
    UnLock(CurrentDir(fl));
    ret=0;
  }
  else
  {
    __seterrno();
    ret=-1;
  }
  return ret;
}

void __initchdir(void)
{
  BPTR fl;
  if (!(fl=Lock("",SHARED_LOCK)))
    exit(20);
  oldlock=CurrentDir(fl);
  havelock=1;
}

void __exitchdir(void)
{
  if (havelock) UnLock(CurrentDir(oldlock));
}

__asm__ (".section .init \n .long __initchdir, 190 \n .section .text\n");
__asm__ (".section .fini \n .long __exitchdir, 190 \n .section .text\n");
