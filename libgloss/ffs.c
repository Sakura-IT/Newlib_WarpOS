/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


int ffs(int mask)
{
  int bit;

  if (mask == 0)
    return 0;
  for (bit=1; !(mask&1); bit++)
    mask >>= 1;

  return bit;
}
