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

long long sum = 0;
mtx_t mtx;
once_flag flag = ONCE_FLAG_INIT;

int thrd_start(void *val);
void func_called_once(void);

typedef struct {
	int value;
} value_t;

int main(void)
{
	thrd_t thr[1000];
	value_t values[1000];
	const long long truesum = 1; // Called once
	size_t i;
	int res;

	if(mtx_init(&mtx, mtx_recursive | mtx_timed) != thrd_success)
		return EXIT_FAILURE;

	for(i = 0; i < 1000; i++) {
		values[i].value = (int)(i+1);
		if(thrd_create(thr+i, thrd_start, values+i) != thrd_success)
			exit(EXIT_FAILURE);
	}

	for(i = 0; i < 1000; i++) {
		thrd_join(thr[i], &res);
	}

	mtx_destroy(&mtx);

	if(truesum != sum)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

void func_called_once(void)
{
	mtx_lock(&mtx);
	sum += 1;
	mtx_unlock(&mtx);
}

int thrd_start(void *val)
{
	(void)val;

	thrd_yield();

	call_once(&flag, func_called_once);

	return 0;
}
