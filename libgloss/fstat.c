/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#pragma pack(2)
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <proto/exec.h>
#pragma pack()

extern int __maxfd;
extern int * __fp;

extern void __seterrno(void);
extern int __mystat(struct stat *buf, struct FileInfoBlock *fib);

int _fstat (int fd, struct stat *st)
{
  struct FileInfoBlock *fib;
  FILE * fp;
  LONG pos,len;
  BPTR handle;

  if (fd > __maxfd || fd < 0 || !(fp = (FILE *)__fp[fd]))
  {
    errno = ENOENT; 
    return -1;
  }

  if (!(handle = fp->_handle))
  {
    errno = ENOENT; 
    return -1;
  }

  if ((fib=(struct FileInfoBlock *)AllocDosObject(DOS_FIB,NULL)) == NULL)
  {
    __seterrno(); return -1;
  }

  memset(st,0,sizeof(*st));
  st->st_mode    = S_IFCHR | 0777;
  st->st_nlink   = 1;
  st->st_blksize = 512;
  /*st->st_blocks  = 0;*/

  if (((struct FileHandle *)BADDR(handle))->fh_Type) {
    if (!ExamineFH(handle,fib)) {
      len = 0; pos = Seek(handle,0,OFFSET_END);
      if (pos >= 0 && (IoErr() != ERROR_ACTION_NOT_KNOWN))
        len = Seek(handle,pos,OFFSET_BEGINNING);
      fib->fib_DiskKey      = (ino_t)~((LONG)handle);
      fib->fib_DirEntryType = -1;
      fib->fib_Size         = len;
      fib->fib_Protection   = 0xff0;
      fib->fib_NumBlocks    = (len+=511,len>>=9);
      DateStamp(&fib->fib_Date);
    }
    __mystat(st,fib);
    if (Seek(handle,0,OFFSET_CURRENT),IoErr())
      st->st_mode = ((st->st_mode & ~S_IFREG) | S_IFCHR);
  }
  FreeDosObject(DOS_FIB,fib); return 0;
}