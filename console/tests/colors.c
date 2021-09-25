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

#include "../include/console.h"
#include <stdio.h>

#if defined(_WIN32) && defined(CONSOLE_FORCE_ANSI)
#include <Windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

extern char *color_name(int c);

int main(void)
{
	int bgr, fgr;
#if defined(_WIN32) && defined(CONSOLE_FORCE_ANSI)
	HANDLE h;
	
	h = GetStdHandle(STD_OUTPUT_HANDLE);
	if(h != INVALID_HANDLE_VALUE) {
		DWORD mode;
		
		if(GetConsoleMode(h, &mode)) {
			mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			SetConsoleMode(h, mode);
		}
	}
#endif
	
	for(bgr = 0; bgr < 16; bgr++)
		for(fgr = 0; fgr < 16; fgr++) {
			consoleColor(fgr, bgr);
			printf("Printing with color %s on background %s\n", color_name(fgr), color_name(bgr));
		}
	
	consoleColor(console_light_gray, console_black);
	
	printf("Normal text\n");
	
	return 0;
}

char *color_name(int c)
{
	switch(c) {
		case console_black:
			return "black";
		case console_blue:
			return "blue";
		case console_green:
			return "green";
		case console_cyan:
			return "cyan";
		case console_red:
			return "red";
		case console_magenta:
			return "magenta";
		case console_brown:
			return "brown";
		case console_light_gray:
			return "light gray";
		case console_dark_gray:
			return "dark gray";
		case console_light_blue:
			return "light blue";
		case console_light_green:
			return "light green";
		case console_light_cyan:
			return "light cyan";
		case console_light_red:
			return "light red";
		case console_light_magenta:
			return "light magenta";
		case console_yellow:
			return "yellow";
		case console_white:
			return "white";
	}
	
	return "";
}
