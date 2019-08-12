/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 */

#include <fcntl.h>
#include <stdio.h>

int creat(const char *path, mode_t mode)
{
  return open(path, O_CREAT | O_TRUNC | O_WRONLY, mode);
}