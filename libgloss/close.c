/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <stdio.h>

#pragma pack(2)
#include <proto/dos.h>
#pragma pack()
#include "bsdsocket.h"

extern void __seterrno(void);
extern int __maxfd;
extern int * __fp;

void __close_socket(int s);

int _close(int file)
{
  int r;
  FILE * fp;	
  if (file < __maxfd && file > 2)
    {
    if (fp = (FILE *)__fp[file])
      {
      if (fp->_nestcnt)
        {
        fp->_nestcnt--;
        return 0;
        }
      if (fp->_fdflags & FDFL_SOCKET)
        __close_socket(fp->_handle);
      else
        {
        r = Close(fp->_handle);
	if (!r)
          {
          __seterrno();
          return -1;
          }
        }
        fp->_handle = 0;
        fp->_fdflags = 0;
        fp->_fcntl = 0;
        __fp[file] = 0;
        return 0;
      }
    }
  errno = ENOENT;
  return -1;
}

