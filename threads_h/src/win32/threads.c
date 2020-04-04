
#include <threads.h>

#include <Windows.h>

#include <process.h>
#include <stdlib.h>

#include <stdio.h>

static DWORD get_time_in_ms(const struct timespec * ts);
typedef struct {
	thrd_start_t thrd_start;
	void *thrd_arg;
	uintptr_t handle;
	int thrd_ret;
	int detached;
} thread_arg_t;
unsigned __stdcall thrd_start_wrapper(void *arg);

// Call once
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
	CloseHandle(*mtx);
}

int mtx_init(mtx_t *mtx, int type)
{
	HANDLE h;

	if(!(type&(mtx_timed|mtx_recursive)))
		return thrd_error;

	h = CreateMutexW(NULL, FALSE, NULL);
	if(h == NULL)
		return thrd_error;

	*mtx = h;

	return thrd_success;
}

int mtx_lock(mtx_t *mtx)
{
	if(WaitForSingleObject(*mtx, INFINITE) == WAIT_OBJECT_0)
		return thrd_success;

	return thrd_error;
}

int mtx_timedlock(mtx_t *restrict mtx, const struct timespec *restrict ts)
{
	DWORD waittime, result;

	waittime = get_time_in_ms(ts);

	result = WaitForSingleObject(*mtx, waittime);

	if(result == WAIT_OBJECT_0)
		return thrd_success;
	else if(result == WAIT_TIMEOUT)
		return thrd_timedout;
	else
		return thrd_error;
}

int mtx_trylock(mtx_t *mtx)
{
	if(WaitForSingleObject(*mtx, 0) == WAIT_OBJECT_0)
		return thrd_success;

	return thrd_error;
}

int mtx_unlock(mtx_t *mtx)
{
	if(ReleaseMutex(*mtx))
		return thrd_success;
	else
		return thrd_error;
}

// Threads
int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	thread_arg_t *thread_arg;

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
	return (thrd_t)0;
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

	return -2;
}

void thrd_yield(void)
{
	Sleep(0);
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

// Non-library functions
static DWORD get_time_in_ms(const struct timespec * ts)
{
	DWORD mstime;

	mstime = ts->tv_sec*1000+ts->tv_nsec/1000000;

	if(ts->tv_nsec % 1000000) mstime++;

	return mstime;
}

unsigned __stdcall thrd_start_wrapper(void *arg)
{
	thread_arg_t *thread_arg;

	thread_arg = arg;

	thread_arg->thrd_ret = thread_arg->thrd_start(thread_arg->thrd_arg);

	if(InterlockedCompareExchange((LONG *)(&(thread_arg->detached)), 1, 1) == 1)
		free(thread_arg);

	return 0;
}
