/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "bsdsocket.h"

int __Pgethostname(char *name, size_t len)
{
  int r;
  if (__init_bsdsocket(-1) >= 0)
    {
    r = gethostname(name,len);
    if (r == -1)
     errno = Errno();
    return r;
    }
  else {
    char *p;
    if (p = getenv("HOSTNAME"))
      {
      if (strlen(p)+1 <= len)
        {
        strcpy(name,p);
        return 0;
        }
      }
    errno = EFAULT;
    }
  return -1;
}
