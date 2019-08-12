/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */

#ifndef BSDAMIGA
#define BSDAMIGA
#endif

#define KERNEL

#pragma pack(push,2)
#include <proto/socket.h>
#pragma pack(pop)

#include "fdflags.h"
#include <sys/types.h>
#include <sys/time.h>

/* Prototypes */

int __init_bsdsocket(int);
void __close_socket(int);
int __socket2fd(int);
int __socket_select(int,fd_set *,fd_set *,fd_set *,
                    struct timeval *,unsigned long *);
