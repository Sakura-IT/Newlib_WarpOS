#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#pragma pack(push,2)
#include <dos/dos.h>
#include <proto/dos.h>
#pragma pack(pop)


int truncate(const char *path, off_t length)
{
	int retval;

	BPTR fd = Open((CONST_STRPTR)path, MODE_OLDFILE);

	if(fd != 0)
	{
		retval = (SetFileSize(fd, length, OFFSET_BEGINNING) >= 0) ? 0 : -1;
		Close(fd);
	}

	else
	{
		errno = ENOENT;
		retval = -1;
	}

	return retval;
}
