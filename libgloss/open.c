/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2006
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#define __LINUX_ERRNO_EXTENSIONS__

#pragma pack(2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack()

#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "prot.h"
#include "fdflags.h"
#include <stdio.h>
#include <errno.h>

extern char *__amigapath(const char *path);
extern int * __fp;
extern int __maxfd;

FILE * __sfp (struct _reent *d);

/* Table to convert amigados error messages to unix ones */

static long _errortable[] = {
  ERROR_NO_FREE_STORE		,ENOMEM,
  ERROR_TASK_TABLE_FULL		,EPROCLIM,
/*ERROR_BAD_TEMPLATE
  ERROR_BAD_NUMBER
  ERROR_REQUIRED_ARG_MISSING
  ERROR_KEY_NEEDS_ARG*/
  ERROR_TOO_MANY_ARGS		,E2BIG,
/*ERROR_UNMATCHED_QUOTES*/
  ERROR_LINE_TOO_LONG		,E2BIG,
/*ERROR_FILE_NOT_OBJECT
  ERROR_INVALID_RESIDENT_LIBRARY
  ERROR_NO_DEFAULT_DIR*/
  ERROR_OBJECT_IN_USE		,ETXTBSY,
  ERROR_OBJECT_EXISTS		,EEXIST,
  ERROR_DIR_NOT_FOUND		,ENOENT,
  ERROR_OBJECT_NOT_FOUND	,ENOENT,
  ERROR_BAD_STREAM_NAME		,ENAMETOOLONG,
  ERROR_OBJECT_TOO_LARGE	,EFBIG,
  ERROR_ACTION_NOT_KNOWN	,ENODEV,
/*ERROR_INVALID_COMPONENT_NAME
  ERROR_INVALID_LOCK
  ERROR_OBJECT_WRONG_TYPE*/
  ERROR_DISK_NOT_VALIDATED	,EBUSY,
  ERROR_DISK_WRITE_PROTECTED	,EROFS,
  ERROR_RENAME_ACROSS_DEVICES	,EXDEV,
  ERROR_DIRECTORY_NOT_EMPTY	,ENOTEMPTY,
  ERROR_TOO_MANY_LEVELS		,ENAMETOOLONG,
  ERROR_DEVICE_NOT_MOUNTED	,ENXIO,
  ERROR_SEEK_ERROR		,ESPIPE,
/*ERROR_COMMENT_TOO_BIG*/
  ERROR_DISK_FULL		,ENOSPC,
  ERROR_DELETE_PROTECTED	,EACCES,
  ERROR_WRITE_PROTECTED		,EACCES,
  ERROR_READ_PROTECTED		,EACCES,
  ERROR_NOT_A_DOS_DISK		,EFTYPE,
  ERROR_NO_DISK			,ENXIO,
/*ERROR_NO_MORE_ENTRIES
  ERROR_IS_SOFT_LINK
  ERROR_OBJECT_LINKED*/
  ERROR_BAD_HUNK		,ENOEXEC,
/*ERROR_NOT_IMPLEMENTED
  ERROR_RECORD_NOT_LOCKED
  ERROR_LOCK_COLLISION
  ERROR_LOCK_TIMEOUT
  ERROR_UNLOCK_ERROR
  ERROR_BUFFER_OVERFLOW
  ERROR_BREAK*/
  ERROR_NOT_EXECUTABLE		,EACCES,
  0				,EPERM
};
  
void __seterrno(void)
{ long amigaerror=IoErr(),*ptr=_errortable,err;

  while((err=*ptr++)&&(err!=amigaerror))
    ++ptr;
  errno=*ptr;
}

int getfd (void)
{
int fd = 3;
while(fd < __maxfd) 
  {
  if (!__fp[fd])
	break;
  ++fd;
  }
  
  if (fd == __maxfd)
    {
    int n = __maxfd + __maxfd + 8;
    __fp = (int*)realloc(__fp, n * sizeof(int));
    if (!__fp) 
      {
	errno = ENOMEM;
	return -1;
      }

    while (__maxfd < n)
	__fp[__maxfd++] = 0;
    }
return fd;
}

int _open_fp (const char *origname, int flags, mode_t mode, FILE *fp)
{  
struct FileInfoBlock fib;  /* longword-aligned! */
  int fd;
  va_list vl;
  BPTR fh=0,lock;
  struct _reent *ptr = _REENT;
  errno = 0;

  if (origname == NULL) {
    errno = EFAULT;
    return -1;
  }

  if ((flags & O_ACCMODE) == O_ACCMODE) {
    /* only O_RDONLY, O_WRONLY or O_RDWR allowed! */
    errno = EINVAL;
    return -1;
  }
  if (!fp)	
    fp = __sfp(_REENT);
  if (!fp)
    return -1;

  STRPTR name = (STRPTR)__amigapath(origname);

  if ((fd = getfd()) < 0)
    return -1;
  flags &= O_ACCMODE|O_NONBLOCK|O_APPEND|O_CREAT|O_TRUNC|O_EXCL;
  fp->_fcntl = flags;

  if (lock = Lock(name,SHARED_LOCK)) {
    if (Examine(lock,&fib)) {
      if (fib.fib_DirEntryType >= 0) {
        /* it's a directory! special handling! */
        if (flags & (O_CREAT|O_TRUNC|O_APPEND|O_RDWR|O_WRONLY)) {
          errno = EISDIR;
          UnLock(lock);
          return -1;
        }
	fp->_handle = lock;
	fp->_fdflags |= FDFL_DIRLOCK;
	__fp[fd] = (int)fp;
        return fd;
      }
    }
    if (!(fh = OpenFromLock(lock)))
      UnLock(lock);
  }
  else if (IoErr() == ERROR_ACTION_NOT_KNOWN) {
    /* handler doesn't support Lock(), so it's not a directory - try Open() */
    fh = Open(name,MODE_OLDFILE);
  }

  if (flags & O_CREAT) {
    if ((flags& O_EXCL) && fh!=0) {   /* file already exists? */
      errno = EEXIST;
      Close(fh);
      return -1;
    }
    if (fh == 0) {
      /* we have to create the file */
      if (fh = Open(name,MODE_NEWFILE)) {
        /* @@@ use ChangeMode() ? */
        Close(fh);
        fh = Open(name,MODE_OLDFILE);
        /* The protection mode has to be set when the file is closed,
           otherwise we might not be able to write to it any longer. */ 
           //amode was here debugdebug
        fp->_fdflags |= FDFL_MODESET;   
      }
      if (fh == 0) {
        __seterrno();
        return -1;
      }
      flags &= ~(O_TRUNC|O_APPEND);  /* don't need those on a fresh file */
    }
  }
  else if (fh == 0) {
    errno = ENOENT;
    return -1;
  }

  if (flags & O_TRUNC) {
    LONG rc = SetFileSize(fh,0,OFFSET_BEGINNING);

    if (IoErr() == ERROR_ACTION_NOT_KNOWN) {
      /* fallback to NEWFILE-method, which might destroy protection flags */
      Close(fh);
      if (fh = Open(name,MODE_NEWFILE)) {
        Close(fh);
        fh = Open(name,MODE_OLDFILE);
      }
      rc = fh ? 0 : -1;
    }
    if (rc == -1) {
      errno = EIO; /* @@@ truncating failed */
      return -1;
    }
    flags &= ~O_APPEND;  /* doesn't make sense on truncated files */
  }

  if (flags & O_APPEND) {
    if (Seek(fh,0,OFFSET_END) == -1) {
      errno = EIO; /* @@@ seek failed */
      Close(fh);
      return -1;
    }
  }

  if (IsInteractive(fh))
    fp->_fdflags |= FDFL_INTERACTIVE;
  fp->_handle = fh;
  __fp[fd] = (int)fp;
  return fd;  
}

int open(const char *origname, int flags, ...)
{
  FILE * fp = NULL;
  mode_t mode;
  va_list vl;
  va_start(vl,flags);
  mode = va_arg(vl,mode_t);
  va_end(vl);

  return _open_fp(origname, flags, mode, fp);
}
