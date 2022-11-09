/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep(int n) {

	STATWORD ps;    

	if (n<0 || clkruns==0) {
		kprintf("SLEEP system error, %d\n", n);
		return(SYSERR);
	}

	if (n == 0) {
	        disable(ps);
		resched();	// caller wishes dispatch call
		restore(ps);
		return(OK);
	}

//  The native MVS-Xinu sleep interval is tenths of seconds

	if (n > 0) sleep10(10*n);
	return(OK);
}

