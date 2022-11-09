/* mark.c - _mkinit, mark */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <sem.h>

#undef	DEBUG
#include <debug.h>

#ifdef	MEMMARK

/*------------------------------------------------------------------------
 *  _mkinit  --  called once at system startup
 *------------------------------------------------------------------------
 */
_mkinit() {

	trace("_MKINIT entered\n");
	trace("_MKINIT configured MAXMARK %d\n", MAXMARK);
	mkmutex = screate(1);
	nmarks = 0;
	trace("_MKINIT exit\n");
}



/*------------------------------------------------------------------------
 *  mark  --  mark a location if it hasn't been marked
 *------------------------------------------------------------------------
 */
SYSCALL mark(int *loc) {

	trace("MARK entered, location 0x%x; mkmutex %d\n", loc, mkmutex);

	if ((*loc >= 0) 
	    && (*loc < nmarks) 
	    && (marks[*loc] == loc) ) {
		trace("MARK return 0 (location marked)\n");
		return(0);
	}

	if (nmarks >= MAXMARK) {
		trace("MARK return SYSERR\n");
		return(SYSERR);
	}

	trace("MARK awaiting mkmutex\n");
	wait(mkmutex);

	marks[ (*loc) = nmarks++] = loc;

	trace("MARK signal mkmutex\n");
	signal(mkmutex);

	trace("MARK return OK\n");
	return(OK);
}
#endif



