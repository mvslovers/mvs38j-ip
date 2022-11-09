/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>

#undef	DEBUG

/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep10(int n) {
	STATWORD ps;    

	if (n < 0  || clkruns==0) {
		kprintf("SLEEP10 system error, %d\n", n);
		return(SYSERR);
	}
	disable(ps);

//  Note: don't set slnempty until sltop is set;
//  MVSTMPOP examines slnempty first

	if (n == 0) {		/* sleep10(0) -> end time slice */
	        ;
	} else {
		insertd(currpid, clockq, n);
		sltop = &q[q[clockq].qnext].qkey;
		slnempty = TRUE;
		proctab[currpid].pstate = PRSLEEP;
	}
#ifdef	DEBUG
	clockstate(NULL, "SLEEP10 before exit");
#endif
	resched();
        restore(ps);
	return(OK);
}





