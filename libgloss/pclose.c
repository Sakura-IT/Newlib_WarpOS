/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include <stdio.h>


int pclose(FILE *fp)
{
  if (fp) {
    /* @@@ we need to wait until the process terminates, and return
       it's exit code @@@ */
    fclose(fp);
    return 0;
  }
  return -1;
}
