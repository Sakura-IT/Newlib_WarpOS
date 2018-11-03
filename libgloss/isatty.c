/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 */


#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <stdlib.h>
#include <stdio.h>

#pragma pack(2)
#include <proto/dos.h>
#pragma pack()

int _isatty(int fd) 
{
	return (unsigned)fd <= 2;
}
