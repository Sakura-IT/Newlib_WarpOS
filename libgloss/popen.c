/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */

#pragma pack(2)
#include <dos/dos.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#include <proto/exec.h>
#pragma pack()
#include <stdio.h>
#include <errno.h>

extern char *__amigapath(const char *path);

FILE *popen(const char *command,const char *mode)
{
  char pipename[32];
  static ULONG id = 0;
  FILE *fp = 0;
  BPTR pipe=0, null=0;

  if (mode[1]!='\0' || (*mode!='r' && *mode!='w')) {
    errno = EINVAL;
    return NULL;
  }

  if (!id)
    id = (ULONG)FindTask(NULL);
  else
    id++;

  /* this requires a working queue-handler! */
  sprintf(pipename,"PIPE:warpos_popen_%08lx",id);
  errno = 0;
  if ((pipe = Open(pipename,MODE_NEWFILE))!=0 &&
      (null = Open("NIL:",MODE_NEWFILE))!=0) {

    switch (SystemTags(__amigapath(command),
                       SYS_Input,*mode=='r'?null:pipe,
                       SYS_Output,*mode=='r'?pipe:null,
                       SYS_Asynch,TRUE,
                       SYS_UserShell,TRUE,
                       TAG_END)) {
      case 0:
        fp = fopen(pipename,mode);
        pipe = null = 0;
        break;
      case -1:
        errno = ENOMEM;
        break;
      default:
        errno = EIO;
        break;
    }
  }
  else
    errno = EIO;

  if (null)
    Close(null);
  if (pipe)
    Close(pipe);

  return fp;
}
