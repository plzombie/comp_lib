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
#include "Windows.h"

void consoleColor(int fgr, int bgr)
{
#if defined(_WIN32) && !defined(CONSOLE_FORCE_ANSI)
	WORD attr = 0;
	HANDLE h;
	
	h = GetStdHandle(STD_OUTPUT_HANDLE);
	if(h == INVALID_HANDLE_VALUE) return;
	
	switch(fgr) {
		case console_blue:
			attr |= FOREGROUND_BLUE;
			break;
		case console_green:
			attr |= FOREGROUND_GREEN;
			break;
		case console_cyan:
			attr |= FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case console_red:
			attr |= FOREGROUND_RED;
			break;
		case console_magenta:
			attr |= FOREGROUND_RED | FOREGROUND_BLUE;
			break;
		case console_brown:
			attr |= FOREGROUND_RED | FOREGROUND_GREEN;
			break;
		case console_light_gray:
			attr |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case console_dark_gray:
			attr |= FOREGROUND_INTENSITY;
			break;
		case console_light_blue:
			attr |= FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			break;
		case console_light_green:
			attr |= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			break;
		case console_light_cyan:
			attr |= FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			break;
		case console_light_red:
			attr |= FOREGROUND_RED | FOREGROUND_INTENSITY;
			break;
		case console_light_magenta:
			attr |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			break;
		case console_yellow:
			attr |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			break;
		case console_white:
			attr |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			break;
	}
	
	switch(bgr) {
		case console_blue:
			attr |= BACKGROUND_BLUE;
			break;
		case console_green:
			attr |= BACKGROUND_GREEN;
			break;
		case console_cyan:
			attr |= BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case console_red:
			attr |= BACKGROUND_RED;
			break;
		case console_magenta:
			attr |= BACKGROUND_RED | BACKGROUND_BLUE;
			break;
		case console_brown:
			attr |= BACKGROUND_RED | BACKGROUND_GREEN;
			break;
		case console_light_gray:
			attr |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case console_dark_gray:
			attr |= BACKGROUND_INTENSITY;
			break;
		case console_light_blue:
			attr |= BACKGROUND_BLUE | BACKGROUND_INTENSITY;
			break;
		case console_light_green:
			attr |= BACKGROUND_GREEN | BACKGROUND_INTENSITY;
			break;
		case console_light_cyan:
			attr |= BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
			break;
		case console_light_red:
			attr |= BACKGROUND_RED | BACKGROUND_INTENSITY;
			break;
		case console_light_magenta:
			attr |= BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
			break;
		case console_yellow:
			attr |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
			break;
		case console_white:
			attr |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
			break;
	}
	
	SetConsoleTextAttribute(h, attr);
#else
	switch(fgr) {
		case console_black:
			printf("\x1b[30m");
			break;
		case console_blue:
			printf("\x1b[34m");
			break;
		case console_green:
			printf("\x1b[32m");
			break;
		case console_cyan:
			printf("\x1b[36m");
			break;
		case console_red:
			printf("\x1b[31m");
			break;
		case console_magenta:
			printf("\x1b[35m");
			break;
		case console_brown:
			printf("\x1b[33m"); // yellow
			break;
		case console_light_gray:
			printf("\x1b[37m");
			break;
		case console_dark_gray:
			printf("\x1b[90m");
			break;
		case console_light_blue:
			printf("\x1b[94m");
			break;
		case console_light_green:
			printf("\x1b[92m");
			break;
		case console_light_cyan:
			printf("\x1b[96m");
			break;
		case console_light_red:
			printf("\x1b[91m");
			break;
		case console_light_magenta:
			printf("\x1b[95m");
			break;
		case console_yellow:
			printf("\x1b[93m");
			break;
		case console_white:
			printf("\x1b[97m");
			break;
	}
	
	switch(bgr) {
		case console_black:
			printf("\x1b[40m");
			break;
		case console_blue:
			printf("\x1b[44m");
			break;
		case console_green:
			printf("\x1b[42m");
			break;
		case console_cyan:
			printf("\x1b[46m");
			break;
		case console_red:
			printf("\x1b[41m");
			break;
		case console_magenta:
			printf("\x1b[45m");
			break;
		case console_brown:
			printf("\x1b[43m"); // Yellow
			break;
		case console_light_gray:
			printf("\x1b[47m");
			break;
		case console_dark_gray:
			printf("\x1b[100m");
			break;
		case console_light_blue:
			printf("\x1b[104m");
			break;
		case console_light_green:
			printf("\x1b[102m");
			break;
		case console_light_cyan:
			printf("\x1b[106m");
			break;
		case console_light_red:
			printf("\x1b[101m");
			break;
		case console_light_magenta:
			printf("\x1b[105m");
			break;
		case console_yellow:
			printf("\x1b[103m");
			break;
		case console_white:
			printf("\x1b[107m");
			break;
	}
#endif
}
