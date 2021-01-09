/*
BSD 2-Clause License

Copyright (c) 2020, Mikhail Morozov
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WIN32
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include "../include/large_files.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

file_t fileOpenA(const char *filename, const char *mode)
{
	return fopen(filename, mode);
}

file_t fileOpenW(const wchar_t *filename, const wchar_t *mode)
{
#ifdef WIN32
	return _wfopen(filename, mode);
#else
	size_t filename_len, mode_len;
	char *cfilename = 0, *cmode = 0;
	file_t f = 0;

	filename_len = wcslen(filename);
	mode_len = wcslen(mode);

	cfilename = malloc(filename_len*MB_CUR_MAX+1);
	cmode = malloc(mode_len*MB_CUR_MAX+1);
	if(!cfilename || !cmode) {
		errno = ENOMEM;
		goto FINAL;
	}

	if(wcstombs(cfilename, filename, filename_len*MB_CUR_MAX+1) == (size_t)(-1)) {
		errno = EINVAL;
		goto FINAL;
	}

	if(wcstombs(cmode, mode, mode_len*MB_CUR_MAX+1) == (size_t)(-1)) {
		errno = EINVAL;
		goto FINAL;
	}

	f = fopen(cfilename, cmode);

FINAL:
	if(cfilename) free(cfilename);
	if(cmode) free(cmode);

	return f;
#endif
}

int fileClose(file_t f)
{
	return fclose(f);
}

int fileSeek(file_t f, int64_t offset, int origin)
{
#ifdef WIN32
	return _fseeki64(f, offset, origin);
#else
	return fseeko64(f, offset, origin);
#endif
}

int64_t fileTell(file_t f)
{
#ifdef WIN32
	return _ftelli64(f);
#else
	return ftello64(f);
#endif
}

int64_t fileLength(file_t f)
{
	int64_t saved_pos, filelen;

	saved_pos = fileTell(f);
	if(saved_pos == -1)
		return -1;

	if(fileSeek(f, 0, SEEK_END) == -1)
		return -1;

	filelen = fileTell(f);

	fileSeek(f, saved_pos, SEEK_SET);

	return filelen;
}

size_t fileRead(void *buffer, size_t size, size_t count, file_t f)
{
	return fread(buffer, size, count, f);
}

size_t fileWrite(void *buffer, size_t size, size_t count, file_t f)
{
	return fwrite(buffer, size, count, f);
}

int fileFlush(file_t f)
{
	return fflush(f);
}
