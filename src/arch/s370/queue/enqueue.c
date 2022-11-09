/* enqueue.c - enqueue */

#include <conf.h>
#include <kernel.h>
#include <q.h>

/*------------------------------------------------------------------------
 * enqueue  --	insert an item at the tail of a list
 *------------------------------------------------------------------------
 */
int	enqueue(item, tail)
	int	item;			/* item to enqueue on a list	*/
	int	tail;			/* index in q of list tail	*/
{
	struct	qent	*tptr;		/* points to tail entry		*/
	struct	qent	*mptr;		/* points to item entry		*/

	tptr = &q[tail];
	mptr = &q[item];
	mptr->qnext = tail;
	mptr->qprev = tptr->qprev;
	q[tptr->qprev].qnext = item;
	tptr->qprev = item;
	return(item);
}

