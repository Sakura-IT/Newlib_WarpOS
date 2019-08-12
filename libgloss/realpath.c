/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2019
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <unistd.h>
#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <sys/types.h>

#pragma pack(push,2)
#include <proto/dos.h>
#include <dos/dos.h>
#pragma pack(pop)

extern char *__amigapath(const char *path);
extern void __seterrno(void);

#define PATH_MAX 256

/****************************************************************************/

/* The following is not part of the ISO 'C' (1994) standard. */

/****************************************************************************/

char *
realpath(const char * path_name, char * buffer)
{
	BPTR lock = 0;
	char *result = NULL;

	if(path_name == NULL || buffer == NULL)
	{
		errno = EFAULT;
		return (result);
	}

	if(path_name[0] == '\0')
	{
		errno = ENOENT;
		return (result);
	}

	STRPTR cpath = (STRPTR)__amigapath(path_name);

	lock = Lock((STRPTR)cpath,SHARED_LOCK);

	if(lock)
	{
		LONG status;
		status = NameFromLock(lock,buffer,PATH_MAX);
		UnLock(lock);

		if(status == DOSFALSE)
		{
			__seterrno();
		}
	}
	else
	{
		__seterrno();
	}

	result = buffer;
	return(result);
}
