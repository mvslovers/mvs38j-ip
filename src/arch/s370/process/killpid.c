/* killpid.c - killpid */

// kill conflicts with Systems/C kill(), renamed to killpid

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>

#define	DEBUG
#undef	DEBUG		// oh, be quiet
#include <debug.h>

#undef	DEBUG_PROCSTATE

/*------------------------------------------------------------------------
 * killpid  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL killpid(int pid) {

	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;


	trace("KILLPID entered; pid %d, currpid %d\n", pid, currpid);
	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		trace("KILLPID error, pid %d pstate %d\n", pid, (&proctab[pid])->pstate);
		return(SYSERR);
	}

	if (--numproc == 0)
		xdone();		// terminate Xinu

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) ) {
//		trace("KILLPID closing device 0\n");
		close(dev);
	}

	dev = pptr->pdevs[1];
	if (! isbaddev(dev) ) {
//		trace("KILLPID closing device 1\n");
		close(dev);
	}

	if ( !isbadpid(pptr->pnxtkin) ) {
		trace("KILLPID notifying next of kin %d\n", pptr->pnxtkin);
		send(pptr->pnxtkin, pid);		// notify next-of-kin
	} else {
		trace("KILLPID no next of kin to notify\n");
	}

	freestk(pptr->pbase, pptr->pstklen);	// release process stack space

	switch (pptr->pstate) {

	case PRDEAD:				// preserve pentry as debug aid
		;				// no need to resched, nulluser will
		break;

	case PRCURR:				// process killing itself
		trace("KILLPID prcurr -> prfree; pid %d\n", pptr->ppid);
#ifdef	MVS_PROC_KEEP_FINI
		pptr->pstate = PRDEAD;		// preserve pentry (debug)
#else
		pptr->pstate = PRFREE;		// recycle pentry
#endif
		trace("KILLPID rescheduling\n");
		resched();
						// fall thru

	case PRWAIT:
		trace("KILLPID prwait; pid %d\n", pptr->ppid);
		semaph[pptr->psem].semcnt++;
						// fall thru

	case PRREADY:
		trace("KILLPID prready; pid %d\n", pptr->ppid);
		dequeue(pid);
		pptr->pstate = PRFREE;
		break;

	case PRSLEEP:
	case PRTRECV:
		trace("KILLPID prsleep or prtrecv; pid %d\n", pptr->ppid);
		unsleep(pid);
						// fall thru

	default:
		trace("KILLPID default; pid %d\n", pptr->ppid);
		pptr->pstate = PRFREE;
	}
	restore(ps);

#ifdef	DEBUG_PROCSTATE
	procstate(NPROC, "KILLPID completed; status");
#endif
	trace("KILLPID exit; killed pid %d\n", pid);
	return(OK);
}







