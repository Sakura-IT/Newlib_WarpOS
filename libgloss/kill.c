/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <signal.h>

#pragma pack(2)
#include <powerpc/powerpc_protos.h>
#include <dos/dos.h>
#pragma pack()

int _kill (int pid, int sig)
{
if(sig== SIGTERM || sig== SIGINT)
  {
  SignalPPC((struct TaskPPC *)pid,SIGBREAKF_CTRL_C);
  return 0;
  }
errno = EINVAL;
return -1;
}

