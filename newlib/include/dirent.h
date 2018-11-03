/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id$
 */

#ifndef _DIRENT_H_
#define _DIRENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/dirent.h>

typedef void *DIR;  /* DIR is private! */


/* Prototypes */

int closedir(DIR *);
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
void rewinddir(DIR *);

#if !defined(_POSIX_C_SOURCE)
int scandir(const char *, struct dirent ***, int (*)(const struct dirent *),
            int (*)(const void *, const void *));
int alphasort(const void *, const void *);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _DIRENT_H_ */
