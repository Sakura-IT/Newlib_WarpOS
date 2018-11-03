/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include <pwd.h>

static int pwindex = 0;

void setpwent(void)
{
  pwindex = 0;
}


struct passwd *getpwent(void)
{
  if (pwindex == 0) {
    pwindex = 1;
    return getpwuid(0);
  }
  return 0;
}


void endpwent(void)
{
}
