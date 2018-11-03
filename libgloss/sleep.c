/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#pragma pack(2)
#include <powerpc/powerpc_protos.h>
#pragma pack()

void sleep(int secs)
{
  if (secs>0)
    {
    WaitTime(0, secs*1000000);
    }
}
