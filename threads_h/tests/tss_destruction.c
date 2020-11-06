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

int thrd_start(void *val);
void tss_dtor(void *);

typedef struct {
	int value;
} value_t;

int main(void)
{
	thrd_t thr;
	int res;

	if(thrd_create(&thr, thrd_start, 0) != thrd_success)
		return EXIT_FAILURE;

	if(thrd_join(thr, &res) != thrd_success)
		return EXIT_FAILURE;

	return res;
}
#include <stdio.h>
int thrd_start(void *val)
{
	size_t i;
	tss_t tss[1000];
	value_t values[1000];
	value_t *v;
	const long long truesum = 500500; // (1+1000)/2*1000 = 1001*500 = 500500

	(void)val;

	for(i = 0; i < 1000; i++) {
		if(tss_create(tss+i, tss_dtor) != thrd_success)
			return EXIT_FAILURE;
	}

	for(i = 0; i < 1000; i++) {
		values[i].value = (int)(i+1);
		if(tss_set(tss[i], values+i) != thrd_success)
			return EXIT_FAILURE;
	}

	for (i = 0; i < 1000; i++) {
		v = tss_get(tss[i]);

		if(v->value != (int)(i + 1))
			return EXIT_FAILURE;

		if(v != values + i)
			return EXIT_FAILURE;
	}

	for(i = 0; i < 1000; i++)
		tss_delete(tss[i]);

	if(truesum != sum)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

void tss_dtor(void *val)
{
	sum += ((value_t *)val)->value;
}
