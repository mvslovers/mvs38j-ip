/* poolinit.c - poolinit */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  poolinit  --  initialize the buffer pool routines
 *------------------------------------------------------------------------
 */
int poolinit() { 

#ifdef	MEMMARK

	int	status;

	trace("POOLINIT entered (memory marking enabled)\n");
	memlock();
	status = mark(bpmark);
	if (status == OK) {
		nbpools = 0;
		trace("POOLINIT exited\n");
		memunlock();
		return(OK);
	} else {
		trace("POOLINIT returning SYSERR\n");
		memunlock();
		BUFPOOL_SYSERR(SYSERR);
	}

#else

	trace("POOLINIT entered (memory marking disabled)\n");
	nbpools = 0;
	trace("POOLINIT exited OK\n");
	return(OK);

#endif
}






