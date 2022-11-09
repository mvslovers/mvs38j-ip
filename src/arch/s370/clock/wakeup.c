/* wakeup.c - wakeup */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>

#undef	DEBUG
#include <debug.h>

#undef	DEBUG_CLOCK

/*------------------------------------------------------------------------
 * wakeup	awaken sleeping processes
 *		Under MVS-Xinu, wakeup returns # process(es) readied
 *------------------------------------------------------------------------
 */
int wakeup() {			// MVS-Xinu returns int; PC-Xinu INTPROC

	int	pid;
	int	awoken = 0;	// number pids marked READY

	trace("WAKEUP scanning clock delta list\n");

#ifdef	DEBUG_CLOCK
	clockstate(NULL, "WAKEUP at ENTRY");
#endif

//  MVS-Xinu clkint/MVSTMPOP decrements the sltop qkey each timer pop

        while (nonempty(clockq) && firstkey(clockq) <= 0) {
		pid = getfirst(clockq);
		trace("WAKEUP awakening process %d\n", pid);
                ready(pid, RESCHNO);
		awoken++;
	}

	if ( slnempty = nonempty(clockq) ) {
		sltop = & q[q[clockq].qnext].qkey;
	}

//  Under MVS-Xinu, SCHED_NO_PREEMPT is defined (meaning we never preempt
//  a process to schedule another).  resched() calls us to scan the sleep
//  queue, so calling resched is a bad idea

#ifdef	SCHED_NO_PREEMPT
	trace("WAKEUP bypassing resched() call; exiting\n");
	return awoken;
#else
	trace("WAKEUP calling resched()\n");
	resched();
	return OK;
#endif

}




