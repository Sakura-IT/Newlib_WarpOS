/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>

struct passwd *getpwuid(uid_t uid)
{
  static struct passwd pw;

  if (uid == 0) {
    /* we only emulate a single user, with uid 0 */
    memset(&pw,0,sizeof(struct passwd));
    pw.pw_name = getlogin();
    pw.pw_passwd = "*";
    pw.pw_shell = "CLI";  /* @@@ */
    pw.pw_dir = getenv("HOME");
    if (pw.pw_dir == NULL)
      pw.pw_dir = "SYS:";  /* default home */
    return &pw;
  }

  return NULL;
}


struct passwd *getpwnam(const char *name)
{
  if (!strcmp(name,getlogin()))
    return getpwuid(0);

  return NULL;
}
