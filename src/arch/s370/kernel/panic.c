/*------------------------------------------------------------------------
 *	panic  --  panic and abort XINU
 *------------------------------------------------------------------------
 */

#include <conf.h>
#include <kernel.h>    
#include <proc.h>
#include <mem.h>
#include <sleep.h>
#include <stdarg.h>		// Standard C header
#include <string.h>		// Standard C header

void panic(const char *msg, ...) {

	va_list ap;  

	kprintf("Panic currpid %d (%s)\n", currpid, proctab[currpid].pname);
	kprintf("Panic reason: %s\n", msg, &ap);
	va_end(ap);

	procstate(NPROC, "PANIC final process status");
	memstate("PANIC final memory status");
	clockstate(NULL, "PANIC final clock status");
	stackstate(NULL, "PANIC final savearea status");

#ifdef	MVS_XDONE_DEBUG
	if ( (strcmp(msg, MVS_XDONE_DEBUG) == 0) )
		return;			// xdone wanted component summary
#endif

	kprintf("PANIC ****************************************** PANIC\n");

#ifdef	MVS_PANIC_ABCOD
	mvsabend("PANIC", MVS_PANIC_ABCOD);
#else
	kprintf("Panic ... MVS-Xinu terminating\n");
#endif

	exit(69);
}











