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

#ifdef WIN32
#include <Windows.h>
#include <ShlObj.h>
#else // LINUX
#include <limits.h>
#include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static int cdirsGetSysDependentPathId(int pathid);
static wchar_t *cdirsGetExeFilenameW(void);
static char *cdirsGetExeFilenameA(void);
static bool cdirsSetExePath(cdirs_data_t *data);
static bool cdirsSetHomePaths(cdirs_data_t *data);

bool cdirsInit(cdirs_data_t *data)
{
	memset(data, 0, sizeof(cdirs_data_t));
	data->size = sizeof(cdirs_data_t);

	if(!cdirsSetExePath(data))
		goto ERROR_STATE;

	if(!cdirsSetHomePaths(data))
		goto ERROR_STATE;

	return true;

ERROR_STATE:
	cdirsDestroy(data);

	return false;
}

void cdirsDestroy(cdirs_data_t *data)
{
	if(data->size != sizeof(cdirs_data_t))
		return;

	if(data->exe_path_w)
		free(data->exe_path_w);
	if(data->exe_path_a)
		free(data->exe_path_a);

	if(data->home_path_w)
		free(data->home_path_w);
	if(data->home_path_a)
		free(data->home_path_a);

	if(data->docs_path_w)
		free(data->docs_path_w);
	if(data->docs_path_a)
		free(data->docs_path_a);

	if(data->pics_path_w)
		free(data->pics_path_w);
	if(data->pics_path_a)
		free(data->pics_path_a);

	if(data->music_path_w)
		free(data->music_path_w);
	if(data->music_path_a)
		free(data->music_path_a);

	memset(data, 0, sizeof(cdirs_data_t));
}

wchar_t *cdirsGetPathW(cdirs_data_t *data, int pathid)
{
	pathid = cdirsGetSysDependentPathId(pathid);

	switch(pathid) {
		case CDIRS_EXE_PATH:
			return data->exe_path_w;
		case CDIRS_HOME_PATH:
			return data->home_path_w;
		case CDIRS_DOCUMENTS_PATH:
			return data->docs_path_w;
		case CDIRS_PICTURES_PATH:
			return data->pics_path_w;
		case CDIRS_MUSIC_PATH:
			return data->music_path_w;
		case CDIRS_VIDEO_PATH:
			return data->video_path_w;
		default:
			return 0;
	}
}

char *cdirsGetPathA(cdirs_data_t *data, int pathid)
{
	pathid = cdirsGetSysDependentPathId(pathid);

	switch(pathid) {
		case CDIRS_EXE_PATH:
			return data->exe_path_a;
		case CDIRS_HOME_PATH:
			return data->home_path_a;
		case CDIRS_DOCUMENTS_PATH:
			return data->docs_path_a;
		case CDIRS_PICTURES_PATH:
			return data->pics_path_a;
		case CDIRS_MUSIC_PATH:
			return data->music_path_a;
		case CDIRS_VIDEO_PATH:
			return data->video_path_a;
		default:
			return 0;
	}
}

// Modifies pathid if it's not available on system and returns it
static int cdirsGetSysDependentPathId(int pathid)
{
	switch(pathid) {
#ifndef WIN32
		case CDIRS_DOCUMENTS_PATH:
		case CDIRS_PICTURES_PATH:
		case CDIRS_MUSIC_PATH:
		case CDIRS_VIDEO_PATH:
			return CDIRS_HOME_PATH;
#endif
		default:
			return pathid;
	}
}

static wchar_t *cdirsGetExeFilenameW(void)
{
	wchar_t *path = 0;

#ifdef WIN32
	DWORD ret;

	path = malloc(MAX_PATH * sizeof(wchar_t));
	if(!path)
		return 0;

	ret = GetModuleFileNameW(NULL, path, MAX_PATH);
	if(ret == MAX_PATH || ret == 0) {
		free(path);

		return 0;
	}
#else
	char *patha;
	patha = cdirsGetExeFilenameA();
	
	if(patha) {
		size_t len, ret;

		len = strlen(patha) + 1;

		path = malloc(len * sizeof(wchar_t));
		if(!path) {
			free(patha);

			return 0;
		}

		ret = mbstowcs(path, patha, len);
		if(ret == len || ret == (size_t)-1) {
			free(patha);
			free(path);

			return 0;
		}

		free(patha);
	}
#endif
	return path;
}

static char *cdirsGetExeFilenameA(void)
{
	char *path;

#ifdef WIN32
	DWORD ret;

	path = malloc(MAX_PATH);
	if(!path)
		return 0;

	ret = GetModuleFileNameA(NULL, path, MAX_PATH);
	if(ret == MAX_PATH || ret == 0) {
		free(path);

		return 0;
	}
#else
	ssize_t ret;

	path = malloc(PATH_MAX);
	if(!path)
		return 0;

	ret = readlink("/proc/self/exe", path, PATH_MAX);
	if(ret == PATH_MAX || ret == -1) {
		free(path);

		return 0;
	}
	path[ret] = 0;
#endif

	return path;
}

static bool cdirsSetExePath(cdirs_data_t *data)
{
	wchar_t *pathw, *nullw, *slashw;
	char *patha, *nulla, *slasha;

	pathw = cdirsGetExeFilenameW();
	patha = cdirsGetExeFilenameA();
	if(!pathw || !patha) {
		if(pathw) free(pathw);
		if(patha) free(patha);

		return false;
	}

	data->exe_path_w = pathw;
	data->exe_path_a = patha;

	// Find end of path for widechar string
	slashw = wcsrchr(pathw, L'/');
	if(!slashw)
		slashw = pathw;
	{
#ifdef WIN32
		wchar_t *bslashw;

		bslashw = wcsrchr(slashw, L'\\');
		if(bslashw)
			nullw = bslashw;
		else
			nullw = slashw;
#else
		nullw = slashw;
#endif
	}

	// Find end of path for multibyte string
	slasha = strrchr(patha, '/');
	if(!slasha)
		slasha = patha;
	{
#ifdef WIN32
		char *bslasha;

		bslasha = strrchr(slasha, '\\');
		if(bslasha)
			nulla = bslasha;
		else
			nulla = slasha;
#else
		nulla = slasha;
#endif
	}

	*nullw = 0;
	*nulla = 0;

	return true;
}

#ifdef WIN32
static bool cdirsGetSHFolder(wchar_t **path_w, char **path_a, int csidl)
{
	*path_w = malloc(MAX_PATH * sizeof(wchar_t));
	*path_a = malloc(MAX_PATH);
	if(!*path_w || !*path_a)
		return false; // pointers will be freed in cdirsDestroy

	if(SHGetFolderPathW(NULL, csidl, NULL, SHGFP_TYPE_CURRENT, *path_w) != S_OK) {
		free(*path_w);
		*path_w = 0;
	}

	if(SHGetFolderPathA(NULL, csidl, NULL, SHGFP_TYPE_CURRENT, *path_a) != S_OK) {
		if(*path_w) {
			free(*path_w);
			*path_w = 0;
		}
		free(*path_a);
		*path_a = 0;
	}

	return true;
}

static bool cdirsSetHomePaths(cdirs_data_t *data)
{
	// Home
	if(!cdirsGetSHFolder(&data->home_path_w, &data->home_path_a, CSIDL_PROFILE))
		return false;

	// Documents
	if(!cdirsGetSHFolder(&data->docs_path_w, &data->docs_path_a, CSIDL_PERSONAL))
		return false;

	// Pictures
	if(!cdirsGetSHFolder(&data->pics_path_w, &data->pics_path_a, CSIDL_MYPICTURES))
		return false;

	// Music
	if(!cdirsGetSHFolder(&data->music_path_w, &data->music_path_a, CSIDL_MYMUSIC))
		return false;

	// Video
	if(!cdirsGetSHFolder(&data->video_path_w, &data->video_path_a, CSIDL_MYVIDEO))
		return false;

	return true;
}
#else
static bool cdirsSetHomePaths(cdirs_data_t *data)
{
	return true;
}
#endif
