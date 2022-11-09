/* sendf.c - sendf */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  sendf  --  sendf a message to another process, forcing delivery
 *------------------------------------------------------------------------
 */
SYSCALL	sendf(int pid, int msg) {
	struct	pentry	*pptr;

	memlock();
	if ( isbadpid(pid) || ((pptr= &proctab[pid])->pstate == PRFREE) ) {
		memunlock();
		return(SYSERR);
	}
	pptr->pmsg = msg;
	pptr->phasmsg++;
	if (pptr->pstate == PRRECV)
		ready(pid, RESCHYES);
	memunlock();
	return(OK);
}

