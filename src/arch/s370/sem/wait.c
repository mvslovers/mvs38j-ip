/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>

#undef	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 * wait  --  	make current process wait on a semaphore
 *		See sem.h for semaphore notes
 *		Wait decrements semcnt
 *		If semcnt non-negative, returns to caller immediately
 *		else marks process PRWAIT & reschedules
 *------------------------------------------------------------------------
 */
SYSCALL	wait(int sem) {

	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;
	dbug(pentry	*pent);
	dbug(char	bufcall[FUNCSTR_BUFLEN]);
	dbug(char	bufhist[FUNCSTR_BUFLEN + 40]);


	sptr = &semaph[sem];
	dbug(pent = &proctab[currpid]);
	dbug(funcstr(bufcall, MY_CALLER));
	trace("\nWAIT enter <<%s>>; pid %d %s semaphore %d; semcnt %d\n", 
		bufcall, currpid, pent->pname, sem, sptr->semcnt);

	dbug(funchist(bufhist, sizeof(bufhist), NULL));
	disable(ps);

	if (isbadsem(sem) || sptr->sstate == SFREE) {
		restore(ps);
		trace("WAIT syserr\n");
		return(SYSERR);
	}
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		trace("WAIT call history: %s\n", bufhist);
		enqueue(currpid, sptr->sqtail);
		trace("WAIT semaphore %d BLOCKED enqueued pid %d %s <<%s>>; rescheduling\n", 
			sem, currpid, pent->pname, bufcall);
		resched();
	}
	restore(ps);
	trace("WAIT call history: %s\n", bufhist);
	trace("WAIT exit OK; pid %d %s <<%s>> HOLDS sem %d\n\n",
		currpid, pent->pname, bufcall, sem);
	return(OK);
}


