/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */

#pragma pack(2)
#include <dos/dosextens.h>
#include <proto/dos.h>
#pragma pack()

#include <errno.h>
#include <stdio.h>
#include "fdflags.h"

extern int * __fp;
extern int __maxfd;

int fsync(int fd)
{
  struct FileHandle *fh;
  FILE * fp;
  
  if (fd > __maxfd || fd < 0 || !(fp = (FILE *)__fp[fd]))
  {
    errno = ENOENT; 
    return -1;
  }

  if (fp->_fdflags & FDFL_SOCKET)
  {  
  errno = EINVAL;
  return -1;
  }

  if(fh = (struct FileHandle *)fp->_handle)
    DoPkt(((struct FileHandle *)BADDR(fh))->fh_Type,ACTION_FLUSH,0,0,0,0,0);
  return 0;
}
