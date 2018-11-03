/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#pragma pack(2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack()

#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include "fdflags.h"

extern int * __fp;

int getfd (void);

int fcntl(int fd, int cmd, ...)
{
  va_list vl;
  int newfd,rc=0;
  FILE * fp;

  if (!(fp = (FILE *)__fp[fd]))
    return -1;

  va_start(vl,cmd);

  switch (cmd) {
    case F_DUPFD:
      if ((newfd = getfd()) < 0) {
        rc = -1;
        break;
      }
      __fp[newfd] = (int)fp;
      fp->_nestcnt++;
      break;

    case F_GETFD:
    case F_SETFD:
      break;            /* close-on-exec() not implemented */

    case F_GETFL:
      rc = fp->_fcntl & (O_NONBLOCK | O_APPEND);
      break;

    case F_SETFL:
      fp->_fcntl |= va_arg(vl,int) & (O_NONBLOCK | O_APPEND);
      if (fp->_fcntl & O_APPEND) {
        if (fp->_fdflags & FDFL_SOCKET) {
          fp->_fcntl &= ~O_APPEND;
          errno = EINVAL;
          return -1;
        }
        Seek(fp->_handle,0,OFFSET_END);
      }
      break;

    default:
      errno = EINVAL;
      rc = -1;
      break;
  }

  va_end(vl);
  return rc;
}
