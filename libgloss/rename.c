/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */

#pragma pack(push,2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack(pop)

#include <sys/syslimits.h>
#include <string.h>
#include <errno.h>

extern char *__amigapath(const char *path);

/* overwrites vclib-rename(), as POSIX requires the "to"-file to be deleted */

int rename(const char *from,const char *to)
{
  char to_dos[PATH_MAX];
  BPTR fl;

  errno = 0;
  if (strlen(to)>=PATH_MAX || strlen(from)>=PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }
  strcpy(to_dos,__amigapath(to));

  /* remove 'to'-file, when existing */
  if (fl = Lock(to_dos,EXCLUSIVE_LOCK)) {
    BOOL deleted;

    UnLock(fl);

    if (!(deleted = DeleteFile(to_dos))) {
      if (IoErr() == ERROR_DELETE_PROTECTED) {
        /* force deletion of target, ignoring protection */
        SetProtection(to_dos,0);
        deleted = DeleteFile(to_dos);
      }
    }
    if (!deleted) {
      switch (IoErr()) {
        case ERROR_DELETE_PROTECTED:
          errno = EPERM; break;
        case ERROR_DISK_WRITE_PROTECTED:
          errno = EROFS; break;
        case ERROR_OBJECT_IN_USE:
          errno = EBUSY; break;
        case ERROR_DIRECTORY_NOT_EMPTY:
          errno = ENOTEMPTY;
        default:
          errno = EIO; break;
      }
      return -1;
    }
  }
  else {
    if (IoErr() != ERROR_OBJECT_NOT_FOUND) {
      errno = EPERM;
      return -1;
    }
  }

  if (!Rename(__amigapath(from),to_dos)) {
    switch (IoErr()) {
      case ERROR_DISK_WRITE_PROTECTED:
        errno = EROFS; break;
      case ERROR_OBJECT_IN_USE:
        errno = EBUSY; break;
      case ERROR_DIR_NOT_FOUND:
      case ERROR_OBJECT_NOT_FOUND:
        errno = ENOENT; break;
      case ERROR_INVALID_COMPONENT_NAME:
        errno = EINVAL; break;
      case ERROR_RENAME_ACROSS_DEVICES:
        errno = EXDEV; break;
      case ERROR_OBJECT_EXISTS:  /* shouldn't happen, we just deleted it */
        errno = EIO; break;
      default:
        errno = EIO; break;
    }
    return -1;
  }

  return 0;
}
