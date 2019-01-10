/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fdflags.h"

#pragma pack(2)
#include <exec/memory.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <powerpc/powerpc_protos.h>
#pragma pack()

extern void __call_exitprocs (int code, void *d);
static int __pathconv;

int __maxfd;
int * __fp;

int __stdin;
int __stdout;
int __stderr;

/* Convert Un*x style pathnames to Amiga OS ones */

char *__amigapath(const char *path)
{
  static char *s1=NULL;
  char *s2,*s3;
  int c;

  if (!__pathconv)
    return (char *)path;

  if (s1!=NULL)
    free(s1);

  s1=strdup(path);
  if (s1==NULL)
    {
    errno=ENOMEM;
    return NULL;
    }
  
  s3=s2=s1; /* Replace multiple following '/' by single ones */
  do
    {
    if (*s2=='/')
      while (s2[1]=='/')
        s2++;
    *s3++=*s2;
    }
  while (*s2++);

  s3=s2=s1; /* Remove single dots '.' as directory names */
  c=1;
  do
    {
    while(c&&s2[0]=='.'&&(s2[1]=='/'||s2[1]=='\0'))
      { s2++;
      if (*s2=='/')
        s2++;
      }
    *s3++=*s2;
    c=0;
    if (*s2=='/')
      c=1;
    }
  while(*s2++);
  
  s3=s2=s1; /* Remove double dots '..' as directory names */
  c=1;
  do
    {
    if (c&&s2[0]=='.'&&s2[1]=='.')
      {
      if(s2[2]=='/')
        s2+=2; 
      else if(s2[2]=='\0')
        {
        *s3++='/';
        s2+=2;
        }
      }
    *s3++=*s2;
    c=0;
    if (*s2=='/')
      c=1;
    } 
  while(*s2++);

  if (*s1=='/') /* Convert names beginning with '/' */
    {
    s3=s2=s1;
    s2++;
    if (*s2=='/'||*s2=='\0') /* The root directory */
      return "SYS:";
    while (*s2!='/'&&*s2!='\0')
      *s3++=*s2++;
    *s3++=':';
    if (*s2=='/')
      s2++;
    do
      *s3++=*s2;
    while (*s2++);
    }
  return s1;
}


void doexit (void)
{
  int code = 0;
    __call_exitprocs (code, NULL);
  if (_GLOBAL_REENT->__cleanup)
    (*_GLOBAL_REENT->__cleanup) (_GLOBAL_REENT);
  return;
}


void __initamigapath(void)
{
  char *s;
  s=getenv("NOIXPATHS"); /* Check explicitly for "1", so we can override it locally */
  if (s&&s[0]=='1'&&s[1]=='\0') /* with 'set NOIXPATHS 0' */
    __pathconv=1;
}

void __init_fd()
{
  __maxfd = 256;
  __fp = (int *)calloc(1, __maxfd * sizeof(int));
  if (!__fp)
    {
    errno = ENOMEM;
    exit(ENOMEM);
    }
  __stdin = Input();
  __stdout = Output();
  __stderr = __stdout;
}


void __exit_fd()
{
  for (int i = 3; i < __maxfd; ++i)
    {
    if (__fp[i])
      close(i);
    }
}


int __mystat(struct stat *buf,struct FileInfoBlock *fib)
{
   buf->st_dev=0;
   buf->st_ino=fib->fib_DiskKey;
   buf->st_mode=
     (fib->fib_DirEntryType<0?S_IFREG:
        (fib->fib_DirEntryType!=ST_SOFTLINK?S_IFDIR:S_IFLNK))|
     (fib->fib_Protection&FIBF_READ?0:S_IRUSR)|
     (fib->fib_Protection&FIBF_WRITE?0:S_IWUSR)|
     (fib->fib_Protection&FIBF_EXECUTE?0:S_IXUSR)|
     (fib->fib_Protection&FIBF_GRP_READ?S_IRGRP:0)|
     (fib->fib_Protection&FIBF_GRP_WRITE?S_IWGRP:0)|
     (fib->fib_Protection&FIBF_GRP_EXECUTE?S_IXGRP:0)|
     (fib->fib_Protection&FIBF_OTR_READ?S_IROTH:0)|
     (fib->fib_Protection&FIBF_OTR_WRITE?S_IWOTH:0)|
     (fib->fib_Protection&FIBF_OTR_EXECUTE?S_IXOTH:0);
   buf->st_nlink=0;
   buf->st_uid=fib->fib_OwnerUID;
   buf->st_gid=fib->fib_OwnerGID;
   buf->st_rdev=0;
   buf->st_size=fib->fib_Size;
   buf->st_atime=buf->st_mtime=buf->st_ctime=
    ((fib->fib_Date.ds_Days+2922)*1440+fib->fib_Date.ds_Minute)*60+
     fib->fib_Date.ds_Tick/TICKS_PER_SECOND;
   buf->st_spare1=buf->st_spare2=buf->st_spare3=0;
   buf->st_blksize=512;
   buf->st_blocks=fib->fib_NumBlocks;
   return 0;
}

/* Replace this with your own if you want */

// char __stdiowin[]="CON://///AUTO/CLOSE/WAIT";
char __stdiowin[]="NIL:";

/* Add these three functions to the lists */

  __asm__ (".section .init \n .long __initamigapath, -30 \n .section .text\n");
  __asm__ (".section .fini \n .long doexit, -5 \n .section .text\n");
  __asm__ (".section .init \n .long __init_fd, -40 \n .section .text\n");
  __asm__ (".section .fini \n .long __exit_fd, -40 \n .section .text\n");
