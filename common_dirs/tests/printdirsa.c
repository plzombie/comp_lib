/*
BSD 2-Clause License

Copyright (c) 2021, Mikhail Morozov
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

#include "../include/common_dirs.h"

#include <locale.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	cdirs_data_t data;
	char *exe_path, *home_path, *docs_path, *pics_path, *music_path, *video_path;

	(void)argc;
	(void)argv;

	setlocale(LC_CTYPE, "");

	if(!cdirsInit(&data)) {
		printf("%s\n", "error: can't init library");

		return 0;
	}

	exe_path = cdirsGetPathA(&data, CDIRS_EXE_PATH);
	printf("Executable directory: \"%s\"\n", exe_path ? exe_path : "NULL");

	home_path = cdirsGetPathA(&data, CDIRS_HOME_PATH);
	printf("Home directory: \"%s\"\n", home_path ? home_path : "NULL");

	docs_path = cdirsGetPathA(&data, CDIRS_DOCUMENTS_PATH);
	printf("Documents directory: \"%s\"\n", docs_path ? docs_path : "NULL");

	pics_path = cdirsGetPathA(&data, CDIRS_PICTURES_PATH);
	printf("Pictures directory: \"%s\"\n", pics_path ? pics_path : "NULL");

	music_path = cdirsGetPathA(&data, CDIRS_MUSIC_PATH);
	printf("Music directory: \"%s\"\n", music_path ? music_path : "NULL");

	video_path = cdirsGetPathA(&data, CDIRS_VIDEO_PATH);
	printf("Video directory: \"%s\"\n", video_path ? video_path : "NULL");

	return 0;
}
