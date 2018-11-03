/*
 * $Id$
 *
 * Adapted for NewLib3 by Dennis van der Boon in 2018
 *
 * :ts=4
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

#include <string.h>
#include <libgen.h>
#include <stdio.h>

#define MAXPATHLEN 256

/****************************************************************************/

/* The following is not part of the ISO 'C' (1994) standard. */

/****************************************************************************/

char *
dirname(char *path)
{
	static char new_path[MAXPATHLEN];
	const char * str;
	char * result;
	size_t len;

	/* An empty path always comes out as the "current directory". */
	str = ".";
	len = 1;

	if(path != NULL && path[0] != '\0')
	{
		/* Strip all trailing slashes. */
		len = strlen(path);
		while(len > 1 && path[len-1] == '/')
			len--;

		/* Is there anything left? */
		if(len > 0)
		{
			size_t i;

			for(i = len-1 ;  ; i--)
			{
				if(path[i] == '/')
				{
					/* Return everything up to, but not including
					   the last slash in the path. That's usually
					   the directory name. */
					str = path;
					len = i;

					/* If that produces an empty string, it means
					   that the entire string consists of slash
					   characters. We'll return only the first. */
					if(i == 0)
						len++;

					break;
				}

				if(i == 0)
					break;
			}
		}
	}

	/* Truncate the path name we can return. This function always returns
	   a valid pointer rather than NULL because some software expects it
	   to do so (I blame the specifications). */
	if(len >= sizeof(new_path))
		len = sizeof(new_path)-1;

	memcpy(new_path,str,len);
	new_path[len] = '\0';

	result = new_path;

	return(result);
}
