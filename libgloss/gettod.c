/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#pragma pack(2)
#include <dos/dos.h>
#include <proto/dos.h>
#include <devices/timer.h>
#pragma pack()

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/timeb.h>

struct timeval;

int _gettimeofday (struct timeval  *ptimeval, void *ptimezone)
{
  if (ptimeval) {
    struct DateStamp t;
    DateStamp(&t); /* Get timestamp */
    ptimeval->tv_sec=((t.ds_Days+2922)*1440+t.ds_Minute)*60+
               t.ds_Tick/TICKS_PER_SECOND;
    ptimeval->tv_usec=(t.ds_Tick%TICKS_PER_SECOND)*1000000/TICKS_PER_SECOND;
  }
    /* since AmigaOS doesn't support timezones, we always return
     * GMT...
     */
  return 0;
}

int ftime(struct timeb *tbp)
{
  struct timeval tv;
  struct timezone tz;
  
  gettimeofday(&tv, &tz);
  
  tbp->time	= tv.tv_sec;
  tbp->millitm	= tv.tv_usec / 1000;
  
  tbp->timezone	= tz.tz_minuteswest;
  tbp->dstflag	= tz.tz_dsttime;
  
    /* Could have just made it 0 as it is currently not supported */
  
  return 0;
}

