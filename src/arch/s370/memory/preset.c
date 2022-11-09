/* preset.c - preset */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <ports.h>

/*------------------------------------------------------------------------
 *  preset  --  reset a port, freeing waiting processes and messages
 *------------------------------------------------------------------------
 */
SYSCALL preset(int portid, void (*dispose)()) {
	struct	pt *ptptr;

	memlock();
	if ( isbadport(portid) ||
#ifdef	MEMMARK
	     unmarked(ptmark) ||
#endif
	     (ptptr= &ports[portid])->ptstate != PTALLOC ) {
		memunlock();
		return(SYSERR);
	}
	_ptclear(ptptr, PTALLOC, dispose);
	memunlock();
	return(OK);
}

