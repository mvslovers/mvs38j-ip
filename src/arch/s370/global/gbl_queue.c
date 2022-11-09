/* gbl_queue.c - define queue global data area */

#include <conf.h>
#include <kernel.h>
#include <q.h>

#ifdef DEFINE_EXTERN
struct	qent	q[NQENT];	// q table (see queue.c)
int	nextqueue;		// next slot in q structure to use
int	rdyhead;		// head of read list
int	rdytail;		// tail of ready list (q indicies)
#endif




