/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include "config.h"
#include "bsdsocket.h"
#include <_ansi.h>
#include <_syslist.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#pragma pack(push,2)
#include <proto/dos.h>
#pragma pack(pop)

extern int __maxfd;
extern int * __fp;

int _isatty (int fd)
{
  FILE * fp;
  if (fd > __maxfd || fd < 0 || !(fp = (FILE *)__fp[fd]))
    {
    errno = EBADF; 
    return 0;
    }
  if (fp->_fdflags & FDFL_SOCKET)
    {
    errno = EINVAL;
    return 0;
    }
  if(!IsInteractive(fp->_handle))
    {
    errno = EINVAL;
    return 0;
    }
  return 1;
}
