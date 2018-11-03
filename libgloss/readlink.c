/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

#pragma pack(2)
#include <proto/dos.h>
#include <dos/dos.h>
#pragma pack()

extern char *__amigapath(const char *path);
extern void __seterrno(void);

int readlink (const char *origpath, char *buf, size_t bufsize)
{
  struct DevProc *dp = NULL;
  BPTR lock = 0;
  int retval = -1;

  STRPTR path = (STRPTR)__amigapath(origpath);

  lock = Open(path, MODE_OLDFILE);
  if (lock == 0)
    {
    __seterrno();
    return retval;
    }

  dp = GetDeviceProc(path, NULL);
  if (dp == NULL)
    {
    errno = EINVAL;
    UnLock(lock);
    return retval;
    }

  retval = ReadLink(dp->dvp_Port, lock, path, buf, bufsize);
  if (retval == TRUE)
    retval = strlen(buf);
  else
    retval = -1;

  FreeDeviceProc(dp);
  UnLock(lock);

  return retval;
}
