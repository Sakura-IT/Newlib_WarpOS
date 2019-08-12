/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>

#pragma pack(push,2)
#include <proto/dos.h>
#include <dos/dos.h>
#pragma pack(pop)

extern void __seterrno(void);
extern char *__amigapath(const char *path);

int _link (char *existing, char *new)
{
  BPTR existing_path_lock = 0;
  LONG status;

  if (existing == NULL || new == NULL)
    {
    errno=EFAULT;
    return -1;
    }
        
  STRPTR cpath = (STRPTR)__amigapath(existing);

  existing_path_lock = Lock((STRPTR)existing,SHARED_LOCK);

  if (existing_path_lock == 0)
    {
    __seterrno();
    return -1;
    }

  status = MakeLink((STRPTR)new,existing_path_lock,LINK_HARD);
	
  if (status == DOSFALSE)
    {
    __seterrno();
    UnLock(existing_path_lock);
    return -1;
    }
	
  UnLock(existing_path_lock);
  return 0;
}
