/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#pragma pack(push,2)
#include <exec/memory.h>
#include <devices/timer.h>
#include <proto/exec.h>
#include <powerpc/powerpc_protos.h>
#pragma pack(pop)

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/times.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <errno.h>

void dotimer(ULONG unit,ULONG timercmd,struct timeval *t)
{ struct PortIO {
    struct timerequest treq;
    struct MsgPort port;
  } *portio;

  if ((portio=(struct PortIO *)AllocVecPPC(sizeof(*portio),MEMF_CLEAR|MEMF_PUBLIC, 32L))) {
    portio->port.mp_Node.ln_Type=NT_MSGPORT;
    if ((BYTE)(portio->port.mp_SigBit=AllocSignal(-1))>=0) {
      portio->port.mp_SigTask=FindTask(NULL);
      NEWLIST(&portio->port.mp_MsgList);
      portio->treq.tr_node.io_Message.mn_Node.ln_Type=NT_REPLYMSG;
      portio->treq.tr_node.io_Message.mn_ReplyPort=&portio->port;
      if (!(OpenDevice(TIMERNAME,unit,&portio->treq.tr_node,0))) {
        portio->treq.tr_node.io_Command=timercmd;
        portio->treq.tr_time.tv_secs =t->tv_secs;
        portio->treq.tr_time.tv_micro=t->tv_micro;
        if (!DoIO(&portio->treq.tr_node)) {
          t->tv_secs=portio->treq.tr_time.tv_secs;
          t->tv_micro=portio->treq.tr_time.tv_micro;
        }
        CloseDevice(&portio->treq.tr_node);
      }
      FreeSignal(portio->port.mp_SigBit);
    }
    FreeVecPPC(portio);
  }
}


void timer(long clock[2])
{
  dotimer(UNIT_VBLANK,TR_GETSYSTIME,(struct timeval *)&clock[0]);
}


static long initclock[2];


void __inittimer()
{
  timer(initclock);
}


int getrusage(int who, struct rusage *rusage)
{
  static struct rusage r0 =
    { {0, 0}, {0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  long clock[2];
  int status = 0;

  switch(who) {
    case RUSAGE_SELF:
      timer(clock);
      clock[0] -= initclock[0];
      clock[1] -= initclock[1];
      if (clock[1] < 0) {
        clock[1] += 1000000;
        clock[0] --;
      }
      memcpy(rusage,&r0,sizeof(struct rusage));
      rusage->ru_utime.tv_sec = clock[0];
      rusage->ru_utime.tv_usec = clock[1];
      break;
    case RUSAGE_CHILDREN:
      memcpy(rusage,&r0,sizeof(struct rusage));
      break;
    default:
      errno = EINVAL;
      status = -1;
  }
  return status;
}

clock_t times(struct tms *tp)
{ struct rusage r;
  time_t tim;

  if (getrusage(RUSAGE_SELF, &r) < 0)
    return (clock_t)-1;

  tp->tms_utime = r.ru_utime.tv_sec * CLK_TCK +
		  (r.ru_utime.tv_usec * CLK_TCK) / 1000000;
  tp->tms_stime = r.ru_stime.tv_sec * CLK_TCK +
  		  (r.ru_stime.tv_usec * CLK_TCK) / 1000000;

  if (getrusage(RUSAGE_CHILDREN, &r) < 0)
    return (clock_t)-1;

  tp->tms_cutime = 0;
  tp->tms_cstime = 0;

  if ((tim=time(NULL)) == (time_t)-1)
    return (clock_t)-1;

  /* The value we are supposed to return does not fit in 32 bits.
     Still, it is useful if you are interested in time differences
     in CLK_TCKths of a second.
  */
  return tim * CLK_TCK;
}

__asm__ (".section .init \n .long __inittimer, 200 \n .section .text\n");
