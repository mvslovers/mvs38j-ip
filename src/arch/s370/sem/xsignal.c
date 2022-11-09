/* xsignal.c - signal */

//  Renamed signal to xsignal; function name conflicts with Systems/C signal()
//  Function name remapped in sem.h

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>

#undef	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 * xsignal  --	signal a semaphore, releasing one waiting process
 *		See sem.h for semaphore notes
 *		Signal increments semcnt, readies first waiting process
 *------------------------------------------------------------------------
 */

SYSCALL xsignal(int sem) {

	STATWORD 	ps;    
	register struct	sentry	*sptr;
	int		pid;
	dbug(pentry	*pent);
	dbug(char	bufcall[FUNCSTR_BUFLEN]);

	disable(ps);
	sptr = &semaph[sem];
	dbug(pent = &proctab[currpid]);
	dbug(funcstr(bufcall, MY_CALLER));
	trace("XSIGNAL enter <<%s>>; semaphore %d semcnt %d pid %d %s\n", 
		bufcall, sem, sptr->semcnt, currpid, pent->pname);

	if (isbadsem(sem) || sptr->sstate == SFREE) {
		restore(ps);
		trace("SIGNAL syserr\n");
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0) {
		pid = getfirst(sptr->sqhead);
		trace("XSIGNAL sem %d semcnt %d; READY pid %d (RESCHYES)\n",
			sem, sptr->semcnt, pid);
		ready(pid, RESCHYES);
	}
	restore(ps);
	trace("XSIGNAL return OK; sem %d semcnt %d\n", sem, sptr->semcnt);
	return(OK);
}






