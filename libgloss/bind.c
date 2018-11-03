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

int __Pbind(int fd, const struct sockaddr *a, socklen_t l)
{
  int s,r;

  if ((s = __init_bsdsocket(fd)) >= 0) {
    r = bind(s,a,l);
    if (r == -1)
     errno = Errno();
    return r;
    }

  return -1;
}
