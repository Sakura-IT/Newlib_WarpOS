/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#pragma pack(push,2)
#include <proto/dos.h>
#include <dos/dos.h>
#pragma pack(pop)

extern void __seterrno(void);
extern int * __fp;
extern int __maxfd;

off_t _lseek (int fd, off_t ptr, int dir)
{
  int r;
  FILE * fp;

  if (fd > __maxfd || fd < 0 || !(fp = (FILE *)__fp[fd]))
  {
    errno = ENOENT; 
    return EOF;
  }

  if (Seek(fp->_handle, ptr, dir==SEEK_SET?OFFSET_BEGINNING:
                     dir==SEEK_END?OFFSET_END:OFFSET_CURRENT)!=EOF)
    if ((r=Seek(fp->_handle, 0, OFFSET_CURRENT))!=EOF)
       return (off_t)r;
   __seterrno();
   return EOF;
}
	

