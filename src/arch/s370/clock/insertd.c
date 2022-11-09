/* insertd.c - insertd */

#include <conf.h>
#include <kernel.h>
#include <q.h>

#undef	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  insertd  --  insert process pid in delta list "head", given its key
 *------------------------------------------------------------------------
 */
int insertd(pid, head, key)
	int	pid;		// process id
	int	head;		// q index of queue head = clockq
	int	key;		// q.qkey = tenths of seconds to sleep
{
	int	next;			// runs through list
	int	prev;			// follows next through list

	trace("INSERTD placing pid %d on sleep queue, key %d\n", pid, key);

#ifdef	DEBUG
	queuestate();
	clockstate(NULL, "INSERTD AT ENTRY");
#endif

	for (prev=head, next=q[head].qnext ;
	     q[next].qkey < key; 
	     prev=next, next=q[next].qnext) {
		key -= q[next].qkey;
		trace("INSERTD adjusted key %d prev %d next %d\n", key, prev, next);
	}

	q[pid].qnext = next;
	q[pid].qprev = prev;
	q[pid].qkey  = key;

	trace("INSERTD &q[pid] 0x%x pid %d -- qnext %d qprev %d qkey %d\n",
		&q[pid], pid, q[pid].qnext, q[pid].qprev, q[pid].qkey);

	q[prev].qnext = pid;
	q[next].qprev = pid;

	trace("INSERTD &q[prev] 0x%x prev %d -- qnext %d qprev %d qkey %d\n",
		&q[prev], prev, q[prev].qnext, q[prev].qprev, q[prev].qkey);
	trace("INSERTD &q[next] 0x%x next %d -- qnext %d qprev %d qkey %d\n",
		&q[next], next, q[next].qnext, q[next].qprev, q[next].qkey);

	if (next < NPROC) {
		trace("INSERTD adjusting q[next].qkey by %d; next %d, NPROC %d\n", 
			key, next, NPROC);
		q[next].qkey -= key;
	}

	trace("INSERTD exit\n");
	return(OK);
}


