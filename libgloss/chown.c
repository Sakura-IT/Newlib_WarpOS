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

#pragma pack(2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack()

extern void __seterrno(void);
extern char *__amigapath(const char *path);

int _chown (const char *path, uid_t owner, gid_t group) __attribute__((alias("chown")));

int
chown (const char *path,
        uid_t owner,
        gid_t group)

{
  struct FileInfoBlock fib;  /* long-word aligned! */
  BPTR lock;
  STRPTR cpath = (STRPTR)__amigapath(path);

  if (lock = Lock(cpath,ACCESS_READ)) {
    if (Examine(lock,&fib)) {
      if (owner == (uid_t)-1)
        owner = (uid_t)fib.fib_OwnerUID;
      if (group == (gid_t)-1)
        group = (gid_t)fib.fib_OwnerGID;
      if (SetOwner(cpath,(((LONG)owner&0xffff)<<16)|((LONG)group&0xffff))) {
        UnLock(lock);
        errno = 0;
        return 0;
      }
      else {
      __seterrno();
      }
    }
    else {
    __seterrno();
    UnLock(lock);
    }
  }
  else {
  __seterrno();
  }
  return -1;
}

