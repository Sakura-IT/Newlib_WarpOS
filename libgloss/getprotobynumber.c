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

struct protoent *__Pgetprotobynumber(int n)
{
  struct protoent *r;
  if (__init_bsdsocket(-1) >= 0)
    {
    r = getprotobynumber(n);
    if (!r)
     errno = Errno();
    return r;
    }
  return 0;
}
