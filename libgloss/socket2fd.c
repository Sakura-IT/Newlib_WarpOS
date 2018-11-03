/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "bsdsocket.h"
#include <stdio.h>

extern int __maxfd;
extern int * __fp;

int getfd (void);
FILE  *__sfp (struct _reent *);

int __socket2fd_r(struct _reent *ptr, int s)
{
  int fd;
  FILE * fp;
  if (s >= 0)
    {
    for (fd=3; fd<__maxfd; fd++)
      {
      if (fp = (FILE *) __fp[fd])
        {
        if (fp->_fdflags & FDFL_SOCKET)
          {
          if (s == fp->_handle)
            return fd;
          }
        }
      }
    fd = getfd();
    if (fd == -1)
      return -1;
    if ((fp = __sfp (ptr)) == NULL)
      return -1;
    fp->_fdflags = FDFL_SOCKET | FDFL_INTERACTIVE;
    fp->_file = fd;
    fp->_handle = s;
    __fp[fd] = (int) fp;
    return fd;
    }
  return -1;
}

int __socket2fd(int s)
{
  return __socket2fd_r (_REENT, s);
}
