/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "bsdsocket.h"


void __close_socket(int s)
{
  if (__init_bsdsocket(-1) >= 0) {
    CloseSocket(s);
  }
}
