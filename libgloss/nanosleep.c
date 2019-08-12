/*
 * nanosleep implementation
 * (C)2006 Diego Casorran <diegocr()users.sf.net>
 * Brought to you under the BSD Licence.
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#pragma pack(2)
#include <devices/timer.h>
#include <proto/dos.h>
#include <powerpc/powerpc_protos.h>
#pragma pack()

#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#define NSEC_PER_USEC   1000            /* nanoseconds per microsecond */
#define USEC_PER_SEC    1000000         /* microseconds per second */
#define NSEC_PER_SEC    1000000000      /* nanoseconds per second */

int nanosleep(struct timespec *requested_time, struct timespec *remaining_time)
{
  struct timeval reqtv;
  struct timeval f,s;
  long rs = 0, ru = 0 ;
  int rc;
  int mydelay;

  if ((requested_time == NULL) || (requested_time->tv_sec < 0)	|| (requested_time->tv_nsec > NSEC_PER_SEC))
    {
    errno = EINVAL;
    return -1;
    }

  if (remaining_time != NULL)
    GetSysTimePPC(&f);

  mydelay = requested_time->tv_sec * 1000000;
  mydelay = mydelay + (requested_time->tv_nsec / 1000);
  rc = WaitTime(SIGBREAKF_CTRL_C, mydelay);

  if (rc != 0)
    {
    if (remaining_time != NULL)
      {
      GetSysTimePPC(&s);
      rs = reqtv.tv_sec	- (s.tv_sec-f.tv_sec);
      ru = reqtv.tv_usec - (s.tv_usec-f.tv_usec);
      rs += ru / 1000000L;
      ru %= 1000000L;
      }

    errno = EINTR;
    rc = -1;
    }

  if (remaining_time != NULL)
    {
    remaining_time->tv_sec = rs;
    remaining_time->tv_nsec = (ru * 1000);
    }
  return(rc);
}
