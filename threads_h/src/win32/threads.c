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

#include <Windows.h>

#include <process.h>
#include <stdlib.h>

static DWORD thrd_tls_thr_t_index = TLS_OUT_OF_INDEXES;

static DWORD get_time_in_ms(const struct timespec * ts);
typedef struct {
	tss_dtor_t dtor;
	void *val;
	int used;
} tss_val_t;
typedef struct {
	tss_val_t *tss_vals;
	size_t tss_vals_max;
	thrd_start_t thrd_start;
	void *thrd_arg;
	uintptr_t handle;
	int thrd_ret;
	int detached;
} thread_arg_t;
typedef struct {
	HANDLE h;
	int type;
	int first_locked;
} mtx_local_t;
static unsigned __stdcall thrd_start_wrapper(void *arg);
static void thrd_release(thread_arg_t *thread_arg);

// Call once
void call_once(once_flag *flag, void (* func)(void))
{
	if(InterlockedCompareExchange((long *)flag, 1, ONCE_FLAG_INIT) == ONCE_FLAG_INIT)
		func();
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

int cnd_timedwait(cnd_t * __restrict cond, mtx_t * __restrict mtx, const struct timespec * __restrict ts)
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
	mtx_local_t *mtx_local;

	mtx_local = *mtx;

	CloseHandle(mtx_local->h);

	free(mtx_local);
}

int mtx_init(mtx_t *mtx, int type)
{
	mtx_local_t *mtx_local;

	mtx_local = malloc(sizeof(mtx_local_t));
	if(!mtx_local)
		return thrd_error;

	mtx_local->type = type;
	mtx_local->first_locked = 0;

	mtx_local->h = CreateMutexW(NULL, FALSE, NULL);
	if(mtx_local->h == NULL) {
		free(mtx_local);

		return thrd_error;
	}

	*mtx = mtx_local;

	return thrd_success;
}

int mtx_lock(mtx_t *mtx)
{
	mtx_local_t *mtx_local;

	mtx_local = *mtx;

	if(WaitForSingleObject(mtx_local->h, INFINITE) == WAIT_OBJECT_0) {
		if(mtx_local->first_locked == 0)
			mtx_local->first_locked = 1;
		else if(!(mtx_local->type&mtx_recursive)){
			ReleaseMutex(mtx_local->h);

			return thrd_error;
		}

		return thrd_success;
	}

	return thrd_error;
}

int mtx_timedlock(mtx_t * __restrict mtx, const struct timespec * __restrict ts)
{
	DWORD waittime, result;
	mtx_local_t *mtx_local;

	mtx_local = *mtx;

	if(!(mtx_local->type&mtx_timed))
		return thrd_error;

	waittime = get_time_in_ms(ts);

	result = WaitForSingleObject(mtx_local->h, waittime);

	if(result == WAIT_OBJECT_0) {
		if(mtx_local->first_locked == 0)
			mtx_local->first_locked = 1;
		else if(!(mtx_local->type&mtx_recursive)) {
			ReleaseMutex(mtx_local->h);

			return thrd_error;
		}

		return thrd_success;
	} else if(result == WAIT_TIMEOUT)
		return thrd_timedout;
	else
		return thrd_error;
}

int mtx_trylock(mtx_t *mtx)
{
	mtx_local_t *mtx_local;
	DWORD result;

	mtx_local = *mtx;

	result = WaitForSingleObject(mtx_local->h, 0);

	if(result == WAIT_OBJECT_0) {
		if(mtx_local->first_locked == 0)
			mtx_local->first_locked = 1;
		else if(!(mtx_local->type&mtx_recursive)){
			ReleaseMutex(mtx_local->h);

			return thrd_busy;
		}

		return thrd_success;
	} else if(result == WAIT_TIMEOUT)
		return thrd_busy;
	else
		return thrd_error;
}

int mtx_unlock(mtx_t *mtx)
{
	mtx_local_t *mtx_local;

	mtx_local = *mtx;

	if(ReleaseMutex(mtx_local->h)) {
		mtx_local->first_locked = 0;

		return thrd_success;
	} else
		return thrd_error;
}

// Threads
int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	thread_arg_t *thread_arg;

	if(thrd_tls_thr_t_index == TLS_OUT_OF_INDEXES) {
		thrd_tls_thr_t_index = TlsAlloc();

		if(thrd_tls_thr_t_index == TLS_OUT_OF_INDEXES)
			return thrd_error;

		thread_arg = malloc(sizeof(thread_arg_t));
		if (!thread_arg) {
			TlsFree(thrd_tls_thr_t_index);
			return thrd_nomem;
		}

		memset(thread_arg, 0, sizeof(thread_arg_t));

		TlsSetValue(thrd_tls_thr_t_index, thread_arg);

		thread_arg = 0;
	}

	thread_arg = malloc(sizeof(thread_arg_t));
	if(!thread_arg)
		return thrd_nomem;

	thread_arg->thrd_arg = arg;
	thread_arg->thrd_start = func;
	thread_arg->thrd_ret = 0;
	thread_arg->detached = 0;
	thread_arg->tss_vals = 0;
	thread_arg->tss_vals_max = 0;

	thread_arg->handle = _beginthreadex(NULL, 0, thrd_start_wrapper, thread_arg, 0, NULL);
	if(thread_arg->handle == 0) {
		free(thread_arg);
		return thrd_error;
	}

	*thr = thread_arg;

	return thrd_success;
}

thrd_t thrd_current(void)
{
	return TlsGetValue(thrd_tls_thr_t_index);
}

int thrd_detach(thrd_t thr)
{
	thread_arg_t *thread_arg;
	HANDLE h;

	thread_arg = thr;

	h = (HANDLE)(thread_arg->handle);

	if(InterlockedCompareExchange((LONG *)(&(thread_arg->detached)), 1, 0) == 0) {
		CloseHandle(h);

		return thrd_success;
	}

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
	thread_arg_t *thread_arg;

	thread_arg = thr;

	if(WaitForSingleObject((HANDLE)(thread_arg->handle), INFINITE) != WAIT_OBJECT_0)
		return thrd_error;

	CloseHandle((HANDLE)(thread_arg->handle));

	if(res)
		*res = thread_arg->thrd_ret;

	thrd_release(thread_arg);

	return thrd_success;
}

int thrd_sleep(const struct timespec *duration, struct timespec *remaining)
{
	DWORD sleeptime;

	sleeptime = get_time_in_ms(duration);

	Sleep(sleeptime); // Sleeps at less sleeptime ms

	if(remaining) {
		remaining->tv_sec = 0;
		remaining->tv_nsec = 0;
	}

	return -2;
}

void thrd_yield(void)
{
	Sleep(0);
}

// Thread local storage
int tss_create(tss_t *key, tss_dtor_t dtor)
{
	thread_arg_t *thread_arg;
	size_t tss_key, new_vals_max;
	tss_val_t *new_vals;

	(void)key;
	(void)dtor;

	thread_arg = (thread_arg_t *)thrd_current();
	if(thread_arg == 0)
		return thrd_error;

	for(tss_key = 0; tss_key < thread_arg->tss_vals_max; tss_key++) {
		if(thread_arg->tss_vals[tss_key].used == 0) {
			thread_arg->tss_vals[tss_key].used = 1;
			thread_arg->tss_vals[tss_key].val = 0;
			thread_arg->tss_vals[tss_key].dtor = dtor;
			*key = (void *)tss_key;

			return thrd_success;
		}
	}

	if(!thread_arg->tss_vals_max)
		new_vals_max = 32;
	else
		new_vals_max = thread_arg->tss_vals_max * 2;

	new_vals = realloc(thread_arg->tss_vals, new_vals_max*sizeof(tss_val_t));
	if(!new_vals)
		return thrd_error;

	for(tss_key = thread_arg->tss_vals_max; tss_key < new_vals_max; tss_key++)
		new_vals[tss_key].used = 0;

	new_vals[thread_arg->tss_vals_max].used = 1;
	new_vals[thread_arg->tss_vals_max].val = 0;
	new_vals[thread_arg->tss_vals_max].dtor = dtor;
	*key = (void *)(thread_arg->tss_vals_max);

	thread_arg->tss_vals = new_vals;
	thread_arg->tss_vals_max = new_vals_max;

	return thrd_success;
}

void tss_delete(tss_t key)
{
	thread_arg_t *thread_arg;
	size_t tss_key;

	thread_arg = (thread_arg_t *)thrd_current();
	if(thread_arg == 0)
		return;

	tss_key = (size_t)key;

	if(tss_key < thread_arg->tss_vals_max) {
		if(thread_arg->tss_vals[tss_key].used) {
			if(thread_arg->tss_vals[tss_key].dtor)
				thread_arg->tss_vals[tss_key].dtor(thread_arg->tss_vals[tss_key].val);
			thread_arg->tss_vals[tss_key].used = 0;
		}
	}
}

void *tss_get(tss_t key)
{
	thread_arg_t *thread_arg;
	size_t tss_key;

	thread_arg = (thread_arg_t *)thrd_current();
	if(thread_arg == 0)
		return 0;

	tss_key = (size_t)key;

	if(tss_key < thread_arg->tss_vals_max) {
		if(thread_arg->tss_vals[tss_key].used)
			return thread_arg->tss_vals[tss_key].val;
	}

	return 0;
}

int tss_set(tss_t key, void *val)
{
	thread_arg_t *thread_arg;
	size_t tss_key;

	thread_arg = (thread_arg_t *)thrd_current();
	if(thread_arg == 0)
		return 0;

	tss_key = (size_t)key;

	if(tss_key < thread_arg->tss_vals_max) {
		if(thread_arg->tss_vals[tss_key].used) {
			thread_arg->tss_vals[tss_key].val = val;

			return thrd_success;
		}
	}

	return thrd_error;
}

// Non-library functions
static DWORD get_time_in_ms(const struct timespec * ts)
{
	DWORD mstime;

	mstime = (DWORD)(ts->tv_sec*1000+ts->tv_nsec/1000000);

	if(ts->tv_nsec % 1000000) mstime++;

	return mstime;
}

static unsigned __stdcall thrd_start_wrapper(void *arg)
{
	thread_arg_t *thread_arg;

	thread_arg = arg;

	TlsSetValue(thrd_tls_thr_t_index, thread_arg);

	thread_arg->thrd_ret = thread_arg->thrd_start(thread_arg->thrd_arg);

	if(InterlockedCompareExchange((LONG *)(&(thread_arg->detached)), 1, 1) == 1)
		thrd_release(thread_arg);

	return 0;
}

static void thrd_release(thread_arg_t *thread_arg)
{
	size_t tss_key;

	for(tss_key = 0; tss_key < thread_arg->tss_vals_max; tss_key++) {
		if(thread_arg->tss_vals[tss_key].used) {
			if(thread_arg->tss_vals[tss_key].dtor)
				thread_arg->tss_vals[tss_key].dtor(thread_arg->tss_vals[tss_key].val);
		}
	}

	free(thread_arg);
}
