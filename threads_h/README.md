# threads.h

threads.h from C11.

Restrictions:
 * timed mutexes and non-recursive mutexes untested (should work),
 * no conditional variables,
 * some strange bugs happens (see mtx_plain test or test from tinycthread),
 * thread specific storage must be allocated for every thread (not like in winapi), need to rewrite it.