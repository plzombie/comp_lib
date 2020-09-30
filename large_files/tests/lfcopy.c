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

#include "../include/large_files.h"

#ifndef _MSC_VER
#ifdef WIN32
#include "../../wmain/src/win32/wmain_win32.c"
#else
#include "../../wmain/src/win32/wmain_stdc.c"
#endif
#endif

#include <stdio.h>
#include <wchar.h>
#include <errno.h>

int wmain(int argc, wchar_t **argv)
{
	size_t bufsize = 32;
	char *buffer;
	file_t infile, outfile;
	int64_t filelen, filecopied;

	if(argc < 3) {
		printf("%s", "lfcopy inputfile outputfile [bufsize=32]\n"
		             "\tcopies inputfile to outputfile\n"
		             "\tbufsize - size of copy buffer in MiB\n");
		return EXIT_SUCCESS;
	}

	if(argc > 3) {
		errno = 0;
		bufsize = wcstol(argv[3], 0, 10);
		if(errno) {
			printf("%s", "error: please enter correct buffer size\n");
			return EXIT_FAILURE;
		}
		if(bufsize > SIZE_MAX/1048576) {
			printf("%s", "error: buffer size is too large\n");
			return EXIT_FAILURE;
		}
	}

	printf("buffer size is %ld MiB\n", (long)bufsize);

	bufsize *= 1048576;
	buffer = malloc(bufsize);
	if(!buffer) {
		printf("%s", "error: can't allocate memory for buffer\n");
		return EXIT_FAILURE;
	}

	infile = fileOpenW(argv[1], L"rb");
	if(!infile) {
		printf("%s", "error: can't open input file\n");
		free(buffer);
		return EXIT_FAILURE;
	}
	outfile = fileOpenW(argv[2], L"wb");
	if(!outfile) {
		printf("%s", "error: can't create output file\n");
		fileClose(infile);
		free(buffer);
		return EXIT_FAILURE;
	}

	filelen = fileLength(infile);
	filecopied = 0;

	while(filecopied < filelen) {
		size_t tobecopied;

		if(bufsize < (uint64_t)(filelen-filecopied)) // diff always > 0
			tobecopied = bufsize;
		else
			tobecopied = (size_t)(filelen-filecopied);

		if(fileSeek(infile, filecopied, SEEK_SET) == -1) { // fileSeek test
			printf("\nerror: can't set input file position\n");
			fileClose(infile);
			fileClose(outfile);
			free(buffer);
			return EXIT_FAILURE;
		}

		if(fileRead(buffer, 1, tobecopied, infile) != tobecopied) {
			printf("\nerror: can't read input file\n");
			fileClose(infile);
			fileClose(outfile);
			free(buffer);
			return EXIT_FAILURE;
		}

		if(fileWrite(buffer, 1, tobecopied, outfile) != tobecopied) {
			printf("\nerror: can't write to output file\n");
			fileClose(infile);
			fileClose(outfile);
			free(buffer);
			return EXIT_FAILURE;
		}

		printf(".");
		filecopied += tobecopied;

		if(filecopied != fileTell(infile)) { // fileTell test
			printf("\nerror: can't get input file position\n");
			fileClose(infile);
			fileClose(outfile);
			free(buffer);
			return EXIT_FAILURE;
		}
	}

	fileClose(infile);
	fileClose(outfile);
	free(buffer);

	return EXIT_SUCCESS;
}
