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

in_addr_t inet_lnaof(struct in_addr in)
{
  in_addr_t r;
  if (__init_bsdsocket(-1) >= 0)
    {
    r = (in_addr_t)Inet_LnaOf((LONG)(in).s_addr);
    if (!r)
     errno = Errno();
    return r;
    }
  return 0; /* @@@ */
}
