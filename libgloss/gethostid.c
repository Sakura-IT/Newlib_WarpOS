/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "bsdsocket.h"

long __Pgethostid(void)
{
  long r;
  if (__init_bsdsocket(-1) >= 0)
    {
    r = (long)gethostid();
    if (!r)
     errno = Errno();
    return r;
    }
  return 0;  /* @@@ shouldn't fail? */
}
