/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#define AMITCP
#include "bsdsocket.h"

in_addr_t __Pinet_addr(const char *n)
{
  in_addr_t r;
  if (__init_bsdsocket(-1) >= 0)
    {
    r = inet_addr(n);
    if (r = INADDR_NONE)
     errno = Errno();
    return r;
    }
  return INADDR_NONE;
}
