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
#include <stdio.h>

mtx_t mtx;

int thrd_start(void *val);


int main(void)
{
	thrd_t thr[1000];
	size_t i;
	int res;

	if(mtx_init(&mtx, mtx_recursive | mtx_timed) != thrd_success)
		return EXIT_FAILURE;

	for(i = 0; i < 1000; i++) {
		if(thrd_create(thr+i, thrd_start, 0) != thrd_success)
			exit(EXIT_FAILURE);
	}

	for(i = 0; i < 1000; i++) {
		thrd_join(thr[i], &res);
		if(res != 0)
			exit(EXIT_FAILURE);
	}

	mtx_destroy(&mtx);

	printf("oki\n");

	return EXIT_SUCCESS;
}

int thrd_start(void *val)
{
	size_t i;

	(void)val;

	for(i = 0; i < 1000; i++) {
		mtx_lock(&mtx);
		if(mtx_trylock(&mtx) != thrd_busy)
			break;
		mtx_unlock(&mtx);
	}

	if(i != 1000) {
		mtx_unlock(&mtx);

		return 1;
	}

	return 0;
}
