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
#pragma pack(2)
#include <exec/libraries.h>
#include <libraries/bsdsocket.h>
#include <workbench/startup.h>
#include <proto/exec.h>
#include <proto/dos.h>
#pragma pack()
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

extern int * __fp;

struct Library *SocketBase = NULL;

const char *getprogname(void);

void _EXIT_4_bsdsocket(void)
{
  if (SocketBase)
    CloseLibrary(SocketBase);
}


int __init_bsdsocket(int s)
{
  static char prgname[256];
  FILE * fp;
  errno = EACCES; /* @@@ */
  if (!SocketBase) {
    struct TagItem tags[3];

    if (SocketBase = OpenLibrary("bsdsocket.library",4)) {
      tags[0].ti_Tag = SBTM_SETVAL(SBTC_ERRNOPTR(sizeof(errno)));
      tags[0].ti_Data = (ULONG)__errno();
      tags[1].ti_Tag = SBTM_SETVAL(SBTC_LOGTAGPTR);
      tags[1].ti_Data = (ULONG)getprogname();
      tags[2].ti_Tag = TAG_DONE;
      if (SocketBaseTagList(tags) != 0) {
        CloseLibrary(SocketBase);
        SocketBase = NULL;
        return -1;
      }
    }
    else
      return -1;
  }

  errno = 0;

  if (s != -1) {
    /* check if 's' is a socket and return AmiTCP socket-fd */
    if (__fp[s]) {
      fp = (FILE *)__fp[s];
      if (fp->_fdflags & FDFL_SOCKET) {
        return fp->_handle;
      }
      else
        errno = ENOTSOCK;
    }
    return -1;
  }

  return 0;
}
  __asm__ (".section .fini \n .long _EXIT_4_bsdsocket, -45 \n .section .text\n");
