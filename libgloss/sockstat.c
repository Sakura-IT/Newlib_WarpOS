/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2010
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include <string.h>
#include <errno.h>
#include <sys/stat.h>

int __sockstat(int bsdsock,struct stat *sb)
{
  /* for a socket all fields are zero, except the mode is _S_IFSOCK */
  memset(sb,0,sizeof(struct stat));
  sb->st_mode = S_IFSOCK;
  errno = 0;
  return 0;
}
