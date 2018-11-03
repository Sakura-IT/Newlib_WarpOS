/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include "fdflags.h"

#pragma pack(2)
#define DEVICES_TIMER_H  /* do not redefine timeval */
struct TimeVal {  /* needed for OS4 */
    unsigned long Seconds;
    unsigned long Microseconds;
};
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack()

/* from bsdsocket.c, we don't want to include bsdsocket.h */
extern int __socket_select(int,fd_set *,fd_set *,fd_set *,
                           struct timeval *,unsigned long *);

/* Wait for this number of microseconds, when polling.
   timer.device might have problems with 0. */
#define POLLING_MICROS 10

/* Microseconds to wait on each file, when there is an infinite timeout. */
#define WAIT_MICROS 1000

extern int __maxfd;
extern int * __fp;

static void amitcp2fd(fd_set *dst,fd_set *src,int nfds)
/* convert AmiTCP socket descriptors to PosixLib descriptors */
{
  FILE * fp;
  int i,j;

  if (dst) {
    for (i=0; i<nfds; i++) {
      if (FD_ISSET(i,src)) {
        for (j=0; j<=__maxfd; j++) {
          if (fp = (FILE *) __fp[j]) {
            if (fp->_fdflags & FDFL_SOCKET) {
              if (i == fp->_handle) {
                FD_SET(j,dst);
                break;
              }
            }
          }
        }
      }
    }
  }
}


static int poll_interactive(fd_set *dst,fd_set *src,int nfds,ULONG timeout)
/* poll all blocking, interactive files with a timeout on each */
{
  FILE * fp;
  int i,n;

  if (timeout < POLLING_MICROS)
    timeout = POLLING_MICROS;
  for (i=0,n=0; i<nfds; i++) {
    if (FD_ISSET(i,src)) {
      if (fp = (FILE *)__fp[i]) {
        if (WaitForChar(fp->_handle,timeout)) {
          if (!(FD_ISSET(i,dst))) {
            FD_SET(i,dst);
            n++;
          }  
        }
      }
    }
  }
  return n;
}


int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout)
{
  int nsock=0,nblock=0,nfile=0,tcp_nfds=0;
  fd_set tcpread,tcpwrite,tcpexcept,blockfds;
  struct timeval polltv;
  FILE * fp;
  int i,n;

  if (nfds > FD_SETSIZE)
    nfds = FD_SETSIZE;
  if (nfds > __maxfd)
    nfds = __maxfd;

  if (timeout) {
    if (timeout->tv_sec < 0 || timeout->tv_usec < 0) {
      errno = EINVAL;
      return -1;
    }
  }
  polltv.tv_sec = 0;
  polltv.tv_usec = POLLING_MICROS;

  FD_ZERO(&tcpread);
  FD_ZERO(&tcpwrite);
  FD_ZERO(&tcpexcept);
  FD_ZERO(&blockfds);
  errno = EBADF;

  if (readfds) {
    /* scan read-fd_set for blocking-, non-blocking files and sockets */

    for (i=0; i<nfds; i++) {
      if (FD_ISSET(i,readfds)) {
         FD_CLR(i,readfds);
        if (fp = (FILE *)__fp[i]) {
          if (fp->_fdflags & FDFL_SOCKET) {
            FD_SET(fp->_handle,&tcpread);
            nsock++;
            if (fp->_handle >= tcp_nfds)
              tcp_nfds = fp->_handle + 1;
          }
          else if (!(fp->_fdflags & FDFL_DIRLOCK)) {
            if (fp->_flags & __SRD) {
              if (fp->_fdflags & FDFL_INTERACTIVE) {
                FD_SET(i,&blockfds);
                nblock++;
              }
              else {
                FD_SET(i,readfds);
                nfile++;
              }
            }
          }
          else
            return -1;  /* EBADF */
        }
        else
          return -1;  /* EBADF */
      }
    }
  }

  if (writefds) {
    /* scan write-fd_set for files and sockets */

    for (i=0; i<nfds; i++) {
      if (FD_ISSET(i,writefds)) {
        FD_CLR(i,writefds);
        if (fp = (FILE *) __fp[i]) {
          if (fp->_fdflags & FDFL_SOCKET) {
            FD_SET(fp->_handle,&tcpwrite);
            nsock++;
            if (fp->_handle >= tcp_nfds)
              tcp_nfds = fp->_handle + 1;
          }
          else if (!(fp->_fdflags & FDFL_DIRLOCK)) {
            if (fp->_flags & __SWR) {
              FD_SET(i,writefds);
              nfile++;
            }
          }
          else
            return -1;  /* EBADF */
        }
        else
          return -1;  /* EBADF */
      }
    }
  }

  if (exceptfds) {
    /* scan exception-fd_set for sockets, ignore other files */

    for (i=0; i<nfds; i++) {
      if (FD_ISSET(i,exceptfds)) {
        FD_CLR(i,exceptfds);
        if (fp = (FILE *) __fp[i]) {
          if (fp->_fdflags & FDFL_SOCKET) {
            FD_SET(fp->_handle,&tcpexcept);
            nsock++;
            if (fp->_handle >= tcp_nfds)
              tcp_nfds = fp->_handle + 1;
          }
        }
        else
          return -1;  /* EBADF */
      }
    }
  }

  errno = 0;

  if (n = nfile) {

    /* There are already non-blocking files ready. So just check the
       sockets and blocking files with zero-timeout, to see home many
       of them are ready too. */

    if (nblock) {
      n += poll_interactive(readfds,&blockfds,nfds,0);
    }
    if (nsock) {
      i = __socket_select(tcp_nfds,&tcpread,&tcpwrite,&tcpexcept,&polltv,0);
      if (i < 0)
        return -1;
      if (i > 0) {
        amitcp2fd(readfds,&tcpread,tcp_nfds);
        amitcp2fd(writefds,&tcpwrite,tcp_nfds);
        amitcp2fd(exceptfds,&tcpexcept,tcp_nfds);
        n += i;
      }
    }
  }

  else {  /* no non-blocking files were in the sets */

    if (nblock && !nsock) {
      /* check interactive, blocking files only */
      unsigned long t;

      if (timeout) {
        t = (timeout->tv_sec > 1000) ?
            1000000000 : (timeout->tv_sec * 1000000 + timeout->tv_usec);
        t /= nblock;
      }
      else
        t = WAIT_MICROS;

      n = poll_interactive(readfds,&blockfds,nfds,0);
      while (n == 0) {
        /* wait sequentially for all files until timeout reached */
        n = poll_interactive(readfds,&blockfds,nfds,t);
        n += poll_interactive(readfds,&blockfds,nfds,0);
        if (timeout)
          break;
      }
    }

    else {
      /* first check if any interactive files are ready */
      if (nblock)
        n = poll_interactive(readfds,&blockfds,nfds,0);

      if (n == 0) {
        if (nblock) {
          if (timeout) {
            i = __socket_select(tcp_nfds,&tcpread,&tcpwrite,&tcpexcept,timeout,0);
          }
          else {
            /* infinte timeout, so check for files in between */
            polltv.tv_usec = WAIT_MICROS;
            while (n+i == 0) {
              i = __socket_select(tcp_nfds,&tcpread,&tcpwrite,&tcpexcept,&polltv,0);
              if (i >= 0)
                n = poll_interactive(readfds,&blockfds,nfds,0);
            }
          }
        }
        else {
          if (nsock) {
            /* sockets only, apply full timeout */
            i = __socket_select(tcp_nfds,&tcpread,&tcpwrite,&tcpexcept,timeout,0);
          }
          else {
            /* no sockets and no files, just wait */
            if (timeout->tv_sec)
              Delay(timeout->tv_sec * TICKS_PER_SECOND);
            usleep(timeout->tv_usec);
            i = 0;
          }
        }
      }
      else {
        /* file data ready, now we only have to poll the sockets */
        i = __socket_select(tcp_nfds,&tcpread,&tcpwrite,&tcpexcept,&polltv,0);
      }

      if (i < 0)
        return -1;
      if (i > 0) {
        amitcp2fd(readfds,&tcpread,tcp_nfds);
        amitcp2fd(writefds,&tcpwrite,tcp_nfds);
        amitcp2fd(exceptfds,&tcpexcept,tcp_nfds);
        n += i;
      }
      if (nblock)
        n += poll_interactive(readfds,&blockfds,nfds,0);
    }
  }

  return n;
}
