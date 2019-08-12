/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <stdlib.h>

#pragma pack(push,2)
#include <powerpc/powerpc_protos.h>
#pragma pack(pop)

int getpid (void)
{
  return (int)FindTaskPPC(NULL);
}

