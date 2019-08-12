/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 *
 * $Id$
 */

#pragma pack(push,2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack(pop)

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fnmatch.h>

extern char *__amigapath(const char *path);

char *__make_ados_pattern(const char *upat,int esc)
{
  char *apat,*p,c;

  if (upat == NULL)
    return NULL;

  if (p = apat = malloc(2*strlen(upat)+1)) {
    if (*upat == '.') {
      if (*(upat+1) == '/')
        upat += 2;
      else if (*(upat+1) == '\0')
        upat++;
    }

    while (c = *upat++) {
      if (c=='.' && *upat=='.' && *(upat+1)=='/') {
        upat++;
        continue;
      }
      else if (c=='/' && *upat=='\0') {
        continue;
      }
      else if (c==0x5c && esc) {
        if (*p++ = *upat++)
          continue;
        else
          break;
      }
      else if (c == '*') {
        *p++ = '#';
        *p++ = '?';
        continue;
      }

      *p++ = c;
    }
    *p++ = '\0';
  }

  return apat;
}


int fnmatch(const char *pattern,const char *string,int flags)
{
  char *adospat,*tokpat;
  LONG len;
  int rc = FNM_NOMATCH;

  errno = ENOMEM;
  if (adospat = __make_ados_pattern(pattern,(flags & FNM_NOESCAPE)==0)) {
    /* tokenize pattern and match against string */
    len = 2*strlen(adospat)+3;
    if (tokpat = malloc(len)) {
      if (ParsePattern((STRPTR)adospat,(STRPTR)tokpat,len) >= 0) {
        errno = 0;
        if (MatchPattern((STRPTR)tokpat,(STRPTR)__amigapath(string))) {
          rc = 0;
        }
      }
      free(tokpat);
    }
    free(adospat);
  }
  return rc;
}
