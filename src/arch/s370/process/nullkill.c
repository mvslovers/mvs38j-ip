// nullkill.c - nullkill

#include <conf.h>
#include <kernel.h>
#include <proc.h>

#define	DEBUG
#undef	DEBUG_STATE			// defined = call <component>state()
#define	DEBUG_NL	""		// "\n" or ""
#include <debug.h>

#undef	DUMP_DEAD			// defined = snap old proc pentry

/*------------------------------------------------------------------------
 *  nullkill	nulluser interface to process management
 *		Terminate completed process
 *	Input:	pointer to terminated process' pentry
 *------------------------------------------------------------------------
 */

void	nullkill(pentry *pptr, int newpid) {

	pentry	*pnew;


#ifdef	DEBUG_STATE
	procstate(NPROC, "NULLKILL");
#endif

	currpid = newpid;			// new current process
	pnew = &proctab[newpid];		// @ pentry
	pnew->pstate = PRCURR;			// mark currently running

	pptr->pflag |= PEFFINI;			// new proc now complete

#ifdef	DUMP_DEAD
	snap("NULLKILL completed pentry", (void *)pptr, sizeof(pentry));
#endif

	trace("NULLKILL pid %d has TERMINATED\n", pptr->ppid);

#ifdef	MVS_PROC_KEEP_ENV
	mvsenv(ENVDESTROY, pptr);	// release Systems/C environment
#else
	trace("NULLKILL preserving pentry 0x%x\n");
#endif

#ifdef	MVS_PROC_KEEP_FINI
	pptr->pstate = PRDEAD;		// don't recycle pentry (debug)
#else
	;				// let killpid recycle PRCURR pentry
#endif

	killpid(pptr->ppid);

#ifdef	DEBUG_STATE
	procstate(NPROC, "NULLKILL status post KILLPID");
#endif

}


