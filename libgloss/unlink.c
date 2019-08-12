/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>

#pragma pack(push,2)
#include <proto/dos.h>
#pragma pack(pop)

extern char *__amigapath(const char *path);
extern void __seterrno(void);

int _unlink (char *name)
{
  if((name=__amigapath(name))==NULL)
    return -1;

  if(DeleteFile((char *)name))
    return 0;
  else
    {
    __seterrno();
    return -1;
    }
}

//int remove(const char *name) __attribute__((alias("unlink")));
int rmdir(const char *name) __attribute__((alias("unlink")));
