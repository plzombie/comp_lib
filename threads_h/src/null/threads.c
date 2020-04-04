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

#include <stdlib.h> // exit(EXIT_FAILURE)

void call_once(once_flag *flag, void (* func)(void))
{
	(void)flag;
	(void)func;
}

// Conditions
int cnd_broadcast(cnd_t *cond)
{
	(void)cond;

	return thrd_error;
}

void cnd_destroy(cnd_t *cond)
{
	(void)cond;
}

int cnd_init(cnd_t *cond)
{
	(void)cond;

	return thrd_error;
}

int cnd_signal(cnd_t *cond)
{
	(void)cond;

	return thrd_error;
}

int cnd_timedwait(cnd_t *restrict cond, mtx_t *restrict mtx, const struct timespec *restrict ts)
{
	(void)cond;
	(void)mtx;
	(void)ts;

	return thrd_error;
}

int cnd_wait(cnd_t *cond, mtx_t *mtx)
{
	(void)cond;
	(void)mtx;

	return thrd_error;
}

// Mutexes
void mtx_destroy(mtx_t *mtx)
{
	(void)mtx;
}

int mtx_init(mtx_t *mtx, int type)
{
	(void)mtx;
	(void)type;

	return thrd_error;
}

int mtx_lock(mtx_t *mtx)
{
	(void)mtx;

	return thrd_error;
}

int mtx_timedlock(mtx_t *restrict mtx, const struct timespec *restrict ts)
{
	(void)mtx;
	(void)ts;

	return thrd_error;
}

int mtx_trylock(mtx_t *mtx)
{
	(void)mtx;

	return thrd_error;
}

int mtx_unlock(mtx_t *mtx)
{
	(void)mtx;

	return thrd_error;
}

// Threads
int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	(void)thr;
	(void)func;
	(void)arg;

	return thrd_error;
}

thrd_t thrd_current(void)
{
	return (thrd_t)0;
}

int thrd_detach(thrd_t thr)
{
	(void)thr;

	return thrd_error;
}

int thrd_equal(thrd_t thr0, thrd_t thr1)
{
	if(thr0 == thr1)
		return -1;
	else
		return 0;
}

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Noreturn void thrd_exit(int res)
#else
void thrd_exit(int res)
#endif
{
	(void)res;

	exit(EXIT_FAILURE);
}

int thrd_join(thrd_t thr, int *res)
{
	(void)thr;
	(void)res;

	return thrd_error;
}

int thrd_sleep(const struct timespec *duration, struct timespec *remaining)
{
	(void)duration;
	(void)remaining;

	return -2;
}

void thrd_yield(void)
{
}

// Thread local storage
int tss_create(tss_t *key, tss_dtor_t dtor)
{
	(void)key;
	(void)dtor;

	return thrd_error;
}

void tss_delete(tss_t key)
{
	(void)key;
}

void *tss_get(tss_t key)
{
	(void)key;

	return 0;
}

int tss_set(tss_t key, void *val)
{
	(void)key;
	(void)val;

	return thrd_error;
}