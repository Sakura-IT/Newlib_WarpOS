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

int __Psendmsg(int fd, const struct msghdr *msg, int flags)
{
  int s,r;
  if ((s = __init_bsdsocket(fd)) >= 0)
    {
    r = sendmsg(s,(struct msghdr *)msg,flags);
    if (r == -1)
     errno = Errno();
    return r;
    }
  return -1;
}
