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

int __Psocket(int domain,int type,int proto)
{
  int r;
  if (__init_bsdsocket(-1) >= 0)
    {
    r = __socket2fd(socket(domain,type,proto));
    if (r == -1)
     errno = Errno();
    return r;
    }
  return -1;
}
