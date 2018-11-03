/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "bsdsocket.h"
#include <stdio.h>

extern int * __fp;

int getfd (void);
FILE  *__sfp (struct _reent *);
int close(int file);

int dup2_r(struct _reent *ptr, int oldd, int newd)
{
  FILE *oldfp, *newfp;

  if (!(oldfp = (FILE *)__fp[oldd]))
    return -1;

  if (newfp = (FILE *)__fp[newd]) 
    close(newd);

  if (oldfp->_fdflags & FDFL_SOCKET) {
    if (__init_bsdsocket(-1) >= 0) {
      FILE * newfp;
      int dupsock;

      dupsock = Dup2Socket(oldfp->_handle, -1);
      if (dupsock >= 0) {
        if (newfp = __sfp (ptr)){          
          newfp->_file = newd;
          newfp->_fdflags = FDFL_SOCKET | FDFL_INTERACTIVE;
          newfp->_handle = dupsock;
          newfp->_fcntl = oldfp->_fcntl;
          __fp[newd] = (int)newfp;
          return newd;
        }
        else
          errno = Errno();
          CloseSocket(dupsock);
      }
    }
    return -1;
  }

  else {
    __fp[newd] = (int)oldfp;
    oldfp->_nestcnt++;
  }
  return newd;
}

int dup2(int oldd, int newd)
{
  return dup2_r (_REENT, oldd, newd);
}
