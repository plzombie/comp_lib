# threads.h

threads.h from C11.

Restrictions:
 * timed mutexes and non-recursive mutexes untested (tests for timed mutexes from tinycthread unpassed),
 * no conditional variables,
 * thread specific storage doesnt support destructors.