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
	thrd_start_t thrd_start;
	void *thrd_arg;
	uintptr_t handle;
	int thrd_ret;
	int detached;
} thread_arg_t;
typedef struct {
	HANDLE h;
	int type;
	size_t nof_locks;
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
	mtx_local->nof_locks = 0;

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
		if(mtx_local->nof_locks == 0)
			mtx_local->nof_locks = 1;
		else if(!(mtx_local->type&mtx_recursive)){
			ReleaseMutex(mtx_local->h);
			
			return thrd_error;
		} else if(mtx_local->nof_locks == SIZE_MAX) {
			ReleaseMutex(mtx_local->h);

			return thrd_error;
		} else
			mtx_local->nof_locks++;

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
		if(mtx_local->nof_locks == 0)
			mtx_local->nof_locks = 1;
		else if(!(mtx_local->type&mtx_recursive)) {
			ReleaseMutex(mtx_local->h);

			return thrd_error;
		} else if(mtx_local->nof_locks == SIZE_MAX) {
			ReleaseMutex(mtx_local->h);

			return thrd_error;
		} else
			mtx_local->nof_locks++;

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
		if(mtx_local->nof_locks == 0)
			mtx_local->nof_locks = 1;
		else if(!(mtx_local->type&mtx_recursive)){
			ReleaseMutex(mtx_local->h);

			return thrd_busy;
		} else if(mtx_local->nof_locks == SIZE_MAX) {
			ReleaseMutex(mtx_local->h);

			return thrd_error;
		} else
			mtx_local->nof_locks++;

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

	mtx_local->nof_locks--;

	if(ReleaseMutex(mtx_local->h))
		return thrd_success;
	else {
		mtx_local->nof_locks++;

		return thrd_error;
	}
}

// Threads
int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	thread_arg_t *thread_arg, *cur_thread;

	if(thrd_tls_thr_t_index == TLS_OUT_OF_INDEXES) {
		thrd_tls_thr_t_index = TlsAlloc();

		if(thrd_tls_thr_t_index == TLS_OUT_OF_INDEXES)
			return thrd_error;

		thread_arg = malloc(sizeof(thread_arg_t));
		if (!thread_arg) {
			TlsFree(thrd_tls_thr_t_index);
			thrd_tls_thr_t_index = TLS_OUT_OF_INDEXES;
			return thrd_nomem;
		}

		memset(thread_arg, 0, sizeof(thread_arg_t));

		TlsSetValue(thrd_tls_thr_t_index, thread_arg);

		thread_arg = 0;
	}

	cur_thread = thrd_current();

	thread_arg = malloc(sizeof(thread_arg_t));
	if(!thread_arg)
		return thrd_nomem;

	thread_arg->thrd_arg = arg;
	thread_arg->thrd_start = func;
	thread_arg->thrd_ret = 0;
	thread_arg->detached = 0;

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
	thread_arg_t* thread_arg;

	thread_arg = thrd_current();

	thrd_release(thread_arg);

	thread_arg->thrd_ret = res;

	_endthreadex(res);
}

int thrd_join(thrd_t thr, int *res)
{
	thread_arg_t *thread_arg;

	thread_arg = thr;

	if(InterlockedCompareExchange((LONG *)(&(thread_arg->detached)), 1, 1) == 1)
		return thrd_error;

	if(WaitForSingleObject((HANDLE)(thread_arg->handle), INFINITE) != WAIT_OBJECT_0)
		return thrd_error;

	CloseHandle((HANDLE)(thread_arg->handle));

	if(res)
		*res = thread_arg->thrd_ret;

	InterlockedCompareExchange((LONG*)(&(thread_arg->detached)), 1, 0);

	thrd_release(thread_arg);

	free(thread_arg);

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

	return 0;
}

void thrd_yield(void)
{
	Sleep(0);
}

// Thread local storage

int tss_create(tss_t *key, tss_dtor_t dtor)
{
	DWORD slot;

	if(dtor)
		return thrd_error;

	slot = TlsAlloc();

	if(slot == TLS_OUT_OF_INDEXES)
		return thrd_error;

	key = (tss_t)slot;

	return thrd_success;
}

void tss_delete(tss_t key)
{
	TlsFree((DWORD)key);
}

void *tss_get(tss_t key)
{
	return TlsGetValue((DWORD)key);
}

int tss_set(tss_t key, void *val)
{
	if(TlsSetValue((DWORD)key, val))
		return thrd_success;
	else
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

	if(InterlockedCompareExchange((LONG *)(&(thread_arg->detached)), 1, 1) == 1) {
		thrd_release(thread_arg);

		free(thread_arg);
	}

	return 0;
}

static void thrd_release(thread_arg_t *thread_arg)
{
	(void)thread_arg;
}
