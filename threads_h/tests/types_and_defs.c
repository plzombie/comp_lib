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

#include <threads.h>

#include <stdlib.h>

void temp_destructor(void *v)
{
	(void)v;
}

int temp_threadfunc(void *v)
{
	(void)v;

	return 0;
}

int main(void)
{
	cnd_t cnd;
	thrd_t thrd;
	tss_t tss;
	mtx_t mtx;
	tss_dtor_t tss_dtor;
	thrd_start_t thrd_start;
	once_flag of;

	int enums;

	(void)cnd;
	(void)thrd;
	(void)tss;
	(void)mtx;
	(void)tss_dtor;
	(void)thrd_start;
	(void)of;
	(void)enums;

	tss_dtor = temp_destructor;
	thrd_start = temp_threadfunc;

	of = ONCE_FLAG_INIT;

	enums = mtx_plain;
	enums = mtx_recursive;
	enums = mtx_timed;

	enums = thrd_timedout;
	enums = thrd_success;
	enums = thrd_busy;
	enums = thrd_error;
	enums = thrd_nomem;

#ifndef thread_local
	return EXIT_FAILURE;
#endif

#ifndef TSS_DTOR_ITERATIONS
	return EXIT_FAILURE;
#endif

	return 0;
}