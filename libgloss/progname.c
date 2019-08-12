/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */


#pragma pack(push,2)
#include <workbench/startup.h>
#include <proto/dos.h>
#pragma pack(pop)

#include <string.h>

char __progname[256];
extern struct WBStartup *_WBenchMsg;  /* from startup-code */

void _INIT_3_progname(void)
{
  if (!GetProgramName(__progname,256))
    {
    if (_WBenchMsg)
      {
      /* @@@ make sure that wa_Name doesn't exceed buffer size!!! */
      strcpy(__progname,(char *)_WBenchMsg->sm_ArgList[0].wa_Name);
      }
    }
}


char *getprogname(void)
{
  return __progname;
}


void setprogname(const char *name)
{
  if (strlen(name) < 256)
    strcpy(__progname,name);
}

  __asm__ (".section .init \n .long _INIT_3_progname, 170 \n .section .text\n");
