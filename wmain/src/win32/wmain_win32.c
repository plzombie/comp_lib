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

#include <Windows.h>

#include <stdlib.h>

int wmain(int argc, wchar_t **argv);

int main(int argc, char **argv)
{
	LPWSTR command_line;
	LPWSTR *wstr_argv_old, *wstr_argv;
	int wstr_argc, i, retval;

	(void)argc;
	(void)argv;

	command_line = GetCommandLineW();

	// Get wchar_t array
	wstr_argv_old = CommandLineToArgvW(command_line, &wstr_argc);
	if(!wstr_argv_old)
		exit(EXIT_FAILURE);

	// Add null value to the end of array
	wstr_argv = malloc((wstr_argc+1)*sizeof(LPWSTR));
	if(!wstr_argv)
		exit(EXIT_FAILURE);

	for(i = 0; i < wstr_argc; i++)
		wstr_argv[i] = wstr_argv_old[i];

	wstr_argv[wstr_argc] = 0;

	LocalFree(wstr_argv_old);

	retval = wmain(wstr_argc, wstr_argv);

	free(wstr_argv);

	return retval;
}
