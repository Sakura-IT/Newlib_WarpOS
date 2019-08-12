/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#pragma pack(push,2)
#include <powerpc/powerpc_protos.h>
#pragma pack(pop)

void sleep(int secs)
{
  if (secs>0)
    {
    WaitTime(0, secs*1000000);
    }
}
