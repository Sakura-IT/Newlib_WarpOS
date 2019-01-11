/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include "bsdsocket.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <stdio.h>

#pragma pack(2)
#include <proto/dos.h>
#include <dos/dos.h>
#pragma pack()

extern int __maxfd;
extern int * __fp;
extern void __seterrno(void);

static struct SignalSemaphorePPC test_sem;

int _write (int fd, char *ptr, int len)
{

  int n,res;
  FILE * fp;

  if (len == 0)
    return 0;

  if (fd > __maxfd || fd < 0 || !(fp = (FILE *)__fp[fd]))
    {
    errno = ENOENT; 
    return -1;
    }

  if (fp->_fdflags & FDFL_SOCKET)
    {
    if ((n = __init_bsdsocket(fd)) >=0)
      {
      res = send(n, ptr, len, 0);
      if (res == -1)
        errno = Errno();
      return res;
      }
    return EOF;
    }

    int r;
    if (fp->_fdflags & FDFL_DIRLOCK)
      {
      errno = EBADF;
      return EOF;
      }

    r=Write(fp->_handle, ptr, len);
    if (r == EOF)
      __seterrno();
    return r;
}

