/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#pragma pack(push,2)
#include <proto/dos.h>
#pragma pack(pop)

extern char *__amigapath(const char *path);
extern void __seterrno(void);

int mkdir(const char *name, mode_t mode)
{
  BPTR fl;
  int ret;
  int len;

  STRPTR cpath = (STRPTR)__amigapath(name);
  
  len = strlen(cpath);
  
  if (cpath[len-1] == *("/"))
    cpath[len-1] = *("\0");     

  if ((fl=CreateDir((STRPTR)cpath)))
    {
    UnLock(fl); ret=chmod(cpath,mode);
    }
  else
    {
    __seterrno(); ret=-1;
    }
  return ret;
}
