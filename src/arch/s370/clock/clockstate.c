/* clockstate.c - Debug Aid - display process table info */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <clock.h>

#ifdef	MVS_GEN_CLOCKSTATE

/*------------------------------------------------------------------------
 * clockstate	display clock management information
 * 		When pid = NULL, report on whole sleep queue
 *		When pid != NULL, report on single pid
 *------------------------------------------------------------------------
 */
void clockstate(int pid, char *msg) {

	int	i, next;

	if (pid == NULL) {
		kprintf("CLOCKSTATE %s\n", (defclk) ? "deferred clock mode" : "non-deferred clock mode");
		kprintf("CLOCKSTATE clockq %d, clkruns %d, cpudelay %d\n", clockq, clkruns, cpudelay);
		kprintf("CLOCKSTATE clmutex %d, clktime %l, clkdiff %d\n", clmutex, clktime, clkdiff);
		kprintf("CLOCKSTATE slnempty %d, sltop 0x%x\n", slnempty, sltop);
		kprintf("CLOCKSTATE preempt %d\n", preempt);
	}

	if (isempty(clockq)) {
		kprintf("CLOCKSTATE queue empty\n");
	} else {
		for (	i = clockq, next = q[clockq].qnext; 
			((next != EMPTY) && (i < NQENT)); 
			i = next, next = q[i].qnext) {
				if ( (pid == NULL) || ( (pid) && (pid = i) ) ) 
					kprintf("<DELTA> &q[i] 0x%x i %d -- qnext %d qprev %d qkey %d\n",
						&q[i], i, q[i].qnext, q[i].qprev, q[i].qkey);
		}
		if (pid == NULL) 
			kprintf("<DELTA> &q[i] 0x%x i %d -- qnext %d qprev %d qkey %d\n",
				&q[i], i, q[i].qnext, q[i].qprev, q[i].qkey);
	}

	if (pid == NULL)
		kprintf("CLOCKSTATE exit\n");

	return;
}

#endif	/* MVS_GEN_CLOCKSTATE */

