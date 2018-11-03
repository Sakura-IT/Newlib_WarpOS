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

struct hostent *__Pgethostbyaddr(const char *a, socklen_t l, int t)
{
  struct hostent *r;
  if (__init_bsdsocket(-1) >= 0)
    {
    r = gethostbyaddr(a,l,t);
    if (!r)
     errno = Errno();
    return r;
    }
  return 0;
}
