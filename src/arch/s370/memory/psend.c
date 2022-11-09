/* psend.c - psend */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <ports.h>
#include <sem.h>

/*------------------------------------------------------------------------
 *  psend  --  send a message to a port by enqueuing it
 *------------------------------------------------------------------------
 */
SYSCALL	psend(int portid, WORD msg) {
	struct	pt	*ptptr;
	int	seq;
	struct	ptnode	*freenode;
	
	memlock();
	if ( isbadport(portid) ||
#ifdef	MEMMARK
	     unmarked(ptmark) ||
#endif
	     (ptptr= &ports[portid])->ptstate != PTALLOC ) {
		memunlock();
		return(SYSERR);
	}

	/* wait for space and verify port is still allocated */

	seq = ptptr->ptseq;
	if (wait(ptptr->ptssem) == SYSERR
	    || ptptr->ptstate != PTALLOC
	    || ptptr->ptseq != seq) {
		memunlock();
		return(SYSERR);
	}
	if (ptfree == NULL)
		panic("Ports  -  out of nodes");
	freenode = ptfree;
	ptfree  = freenode->ptnext;
	freenode->ptnext = (struct ptnode *) NULL;
	freenode->ptmsg  = msg;
	if (ptptr->pttail == (struct ptnode *) NULL)	/* empty queue */
		ptptr->pttail = ptptr->pthead = freenode;
	else {
		(ptptr->pttail)->ptnext = freenode;
		ptptr->pttail = freenode;
	}
	signal(ptptr->ptrsem);
	memunlock();
	return(OK);
}

