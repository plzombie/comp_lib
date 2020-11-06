# threads.h

threads.h from C11.

Restrictions:
 * timed mutexes and non-recursive mutexes untested (should work, tests from tinycthread passed),
 * no conditional variables,
 * thread specific storage works like array, need to rewrite it.