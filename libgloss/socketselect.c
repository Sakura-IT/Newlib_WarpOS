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

int __socket_select(int nfds,fd_set *rd,fd_set *wr,fd_set *ex,
                    struct timeval *timeout,unsigned long *sigmask)
{
  int r;
  if (__init_bsdsocket(-1) >= 0)
    {
    r = WaitSelect(nfds,rd,wr,ex,timeout,sigmask);
    if (r == -1)
     errno = Errno();
    return r;
    }
  return 5;
}
