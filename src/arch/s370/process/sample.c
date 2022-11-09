/* sample.c - sample */

//  Sample process to aid in debugging

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sleep.h>

#undef	DEBUG
#include <debug.h>

PROCESS sample() {

	kprintf("\nSAMPLE ***** ENTER; pid %d\n", currpid);

#ifdef	DEBUG
	stackstate(NULL, "SAMPLE ENTER");
#endif

	sleep(1);		// suspend for 1 second

	kprintf("\nSAMPLE ***** EXIT;  pid %d\n", currpid);

#ifdef	DEBUG
	stackstate(NULL, "SAMPLE prior to exit");
	snap("SAMPLE - SAMPLE pentry", &proctab[currpid], sizeof(pentry));
	snap("SAMPLE - NULLUSER pentry", &proctab[currpid], sizeof(pentry));
	procstate(NPROC, "SAMPLE before exit");
#endif

	return 0;

} /* sample */


