/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <stdlib.h>

#pragma pack(2)
#include <powerpc/powerpc_protos.h>
#pragma pack()

int getpid (void)
{
  return (int)FindTaskPPC(NULL);
}

