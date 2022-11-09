/* pdelete.c - pdelete */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <ports.h>

/*------------------------------------------------------------------------
 *  pdelete  --  delete a port, freeing waiting processes and messages
 *------------------------------------------------------------------------
 */
SYSCALL pdelete(int portid, void (*dispose)()) {
	struct	pt *ptptr;

	memlock();
	if ( isbadport(portid) ||
#ifdef	MEMMARK
	     unmarked(ptmark) ||
#endif
	     (ptptr= &ports[portid])->ptstate != PTALLOC ) {
		memunlock();
		return SYSERR;
	}
	_ptclear(ptptr, PTFREE, dispose);
	memunlock();
	return OK;
}

