/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include "bsdsocket.h"

extern int * __fp;

int ioctl(int fd, unsigned long request, ...)
{
  FILE * fp;
  va_list vl;
  char *arg;

  if (!(fp = (FILE *) __fp[fd]))
    return -1;

  va_start(vl,request);
  arg = va_arg(vl,char *);
  va_end(vl);

  if (fp->_fdflags & FDFL_SOCKET) {
    int s,r;
    if ((s = __init_bsdsocket(fd)) >= 0) {
      r = IoctlSocket(s,request,arg);
      if (r == -1)
        errno = Errno();
      return r;
    }
  }
  else {
    /* @@@ ioctl is currently only supported for sockets */
    errno = EINVAL;
  }
  return -1;
}
