/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */


#include <errno.h>
#include "prot.h"

#pragma pack(push,2)
#include <dos/dos.h>
#pragma pack(pop)

extern char *__amigapath(const char *path);

int chmod(const char *path,mode_t mode)
{
  errno = 0;
  STRPTR cpath = (STRPTR)__amigapath(path);
  if (__set_prot((STRPTR)cpath, mode) < 0)
    {
    errno = EACCES;
    return -1;
    }
  return 0;
}
