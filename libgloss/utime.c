/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */

#pragma pack(2)
#include <proto/dos.h>
#include <devices/timer.h>
#pragma pack()

#include <sys/time.h>
#include <utime.h>
#include <time.h>
#include <utime.h>
#include <errno.h>


#define SECSPERDAY (24L * 60L * 60L)

int utime(const char *file, const struct utimbuf *timep)
{ struct timeval times[2],*t=NULL;

  if (timep) {
    t = &times[0];
    t[0].tv_sec  = timep->actime;
    t[0].tv_usec = 0;
    t[1].tv_sec  = timep->modtime;
    t[1].tv_usec = 0;
  }

  return utimes(file,t);
}

int utimes(const char *file, const struct timeval *times)
{ struct DateStamp d;

  if (times) {
    struct timeval modtime = times[1];
    time_t t;

    if (modtime.tv_usec > 1000000) {
      modtime.tv_sec += (modtime.tv_usec / 1000000);
      modtime.tv_usec %= 1000000;
    }
    t = modtime.tv_sec;
#if 0
    extern long timezone;
#endif
    t -= (2922L * SECSPERDAY);	/* Datestamps start from Jan 1, 1978, unix
				   time from Jan 1, 1970, i.e., 2922 days
				   earlier! */
#if 0
    t -= timezone;	/* Adjust for local time */
#endif
    d.ds_Days = t / SECSPERDAY;
    t %= SECSPERDAY;
    d.ds_Minute = t / 60L;
    t %= 60L;
    d.ds_Tick = t * 50 + (modtime.tv_usec / 20000);
  }else{
    DateStamp(&d);
  }

  if (SetFileDate((STRPTR)file, &d)) {
    errno = 0;
    return 0;
  }else{
    errno = ENOENT;
    return -1;
  }
}
