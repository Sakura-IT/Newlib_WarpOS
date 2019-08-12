/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#pragma pack(push,2)
#include <dos/dos.h>
#include <powerpc/powerpc_protos.h>
#pragma pack(pop)

#include <errno.h>
#include <signal.h>

/**
 * This isn't a 100% implementation since alarm()/SIGALARM isnt supported
 * (yet!) on this current stdlib for amigift ...
 *
 * man:
 * http://www.opengroup.org/onlinepubs/007908799/xsh/usleep.html
 */

int usleep(unsigned long usec)
{
  int result;

  if (usec == 0 || usec >= 1000000 )
    {
    errno = EINVAL;
    return -1;
    }

    result = WaitTime(SIGBREAKF_CTRL_C, usec );

    if (result & SIGBREAKF_CTRL_C )
      {
      raise(SIGABRT);
      errno = EINTR;
      return -1;
      }
  return 0;
}
