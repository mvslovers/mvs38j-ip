/* dequeue.c - dequeue */

#include <conf.h>
#include <kernel.h>
#include <q.h>

/*------------------------------------------------------------------------
 *  dequeue  --  remove an item from the head of a list and return it
 *------------------------------------------------------------------------
 */
int	dequeue(item)
	int	item;
{
	struct	qent	*mptr;		/* pointer to q entry for item	*/

	mptr = &q[item];
	q[mptr->qprev].qnext = mptr->qnext;
	q[mptr->qnext].qprev = mptr->qprev;
	return(item);
}

