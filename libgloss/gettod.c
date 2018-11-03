/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/time.h>
#include <sys/times.h>

#pragma pack(2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack()

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


