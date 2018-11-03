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

int __Psendto(int fd, const void *buf, size_t len, int flags,
              const struct sockaddr *to, socklen_t tolen)
{
  int s,r;
  if ((s = __init_bsdsocket(fd)) >= 0)
    {
    r = sendto(s,buf,len,flags,to,tolen);
    if (r == -1)
     errno = Errno();
    return r;
    }
  return -1;
}
