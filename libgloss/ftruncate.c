/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */


#include "config.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#pragma pack(push,2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack(pop)

extern int * __fp;
extern int __maxfd;

extern void __seterrno(void);


int ftruncate(int fd, int newsize)
{
  FILE * fp;

  if (fd > __maxfd || fd < 0 || !(fp = (FILE *)__fp[fd]))
  {
    errno = ENOENT; 
    return -1;
  }

  int result = SetFileSize(fp->_handle,newsize,OFFSET_BEGINNING);
  if (result != -1)
    return 0;
  else 
    {
    __seterrno();
    return(result);
    }
}
