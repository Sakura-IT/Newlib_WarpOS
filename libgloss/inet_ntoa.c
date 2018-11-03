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

char *inet_ntoa(struct in_addr in)
{
  char *r;
  if (__init_bsdsocket(-1) >= 0)
    {
    r = Inet_NtoA((ULONG)(in).s_addr);
    if (!r)
     errno = Errno();
    return r;
    }
  return 0; /* @@@ */
}
