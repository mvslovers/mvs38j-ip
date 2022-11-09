/* mvslock.c - MVS-specific lock management support */

#ifdef	MVS_LOCKMGR

#include <conf.h>
#include <kernel.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * mvslock - seize control of lock
 *------------------------------------------------------------------------
 */
void mvslock(int which, char *who) {
	__register(6)   void * r6;	// lock value
	__register(7)   void * r7;	// @ pentry.lock
	__register(8)   void * r8;	// pentry.lock when busy
	__register(9)   void * r9;	// lock.time
	__register(15)   void * r15;	// debug aid
	int		nlocks;

#ifdef MVS_TRACE_LOCK
	kprintf("MVSLOCK entry; caller %s lock %d, pid %d\n", 
		who, which, currpid);
#endif
	if ( (which < 0) || (which > MAXLOCK)) {
		panic("mvslock: lock number");
	}

//  Lock the pentry control block so we can safely update nlocks.
//  This lock word should _always_ be free, as long as there is
//  only one TCB running MVS-Xinu code, and no asynchronous code
//  requires locks (as in timer pops).

	r7 = &(proctab[currpid].lock);	// @ pentry lockword
	__asm {
         ICM   15,15,=C'LOCK'     debug aid
         BALR  6,0                convenient lock value
         ULOCK 6,0(7),OLD=8,BUSY=NOTAVAIL
         SR    6,6                indicate lock held
NOTAVAIL DS    0H
	}
	if (r6 != 0) 
		mvsabend("mvslock pentry.lock busy", 100);

//  If we didn't ABEND, we now hold pentry.lock,
//  meaning we own the process entry.  Holding pentry.lock should
//  prevent us from losing control via the dispatcher.

//  Attempt to seize the caller-requested lock.
//  While pentry.lock protects kernel code from the dispatcher,
//  specific k-locks provide diagnostic information as to what activity
//  is in progress.

#ifdef MVS_TRACE_LOCK
	kprintf("MVSLOCK lock %d owner = %d 0x%x\n", 
		which, locks[which].owner, locks[which].owner);
#endif
	r6 = currpid;
	if (r6 == 0) 
		r6 = LOCKNULL;		// don't want pid=0
	r7 = &(locks[which].owner);	// @ lockword caller wants
//	r8 = 0;				// ULOCK sets r8 = 0 before CS
	r9 = &(locks[which].time);	// STCK target
	__asm {
         ICM   15,15,=C'LOCK'		// debug aid
         C     6,0(7)			// does pid already own lock?
         BE    OWN			// yes, br
SEIZE    ULOCK 6,0(7),OLD=8,BUSY=BUSY	// seize caller-requested lock
OWN      STCK  0(9)                     // indicate when owned
         SR    6,6			// tell C code lock owned
BUSY     DS    0H
	}
	if (r6 == 0) {				// pid now owns lock
		locks[which].count++;
		proctab[currpid].nlocks++;
		nlocks = proctab[currpid].nlocks;
		proctab[currpid].lock = 0;	// release pentry lock
#ifdef MVS_TRACE_LOCK
		kprintf("MVSLOCK exit; locked lock %d, pid %d proc %s nlocks %d\n", 
			which, currpid, proctab[currpid].pname, nlocks);
#endif
		return;
	}

//  Lock was not available, issue diagnostics & produce dump

	kprintf("MVSLOCK k-lock %d error, pid %d proc %s\n", 
		which, currpid, proctab[currpid].pname);
	snap("MVSLOCK locks", (void *) &locks, sizeof(locks));
	snap("MVSLOCK pentry", (void *) &proctab[currpid], sizeof(struct pentry));

//*  When we fail to seize the desired lock, it means the dispatch
//*  mechanism is broken, or a manager tried to seize a lock it either
//*  shouldn not hold, or already holds.  
//*  No process should ever lose control when locks are held, with the
//*  sole exception of asynchronous code which should never attempt
//*  to seize any of the k-locks.

	mvsabend("mvslock k-lock not available", 101);

	__asm {
         LTORG ,
	}

} /* mvslock */

#endif	/* MVS_LOCKMGR */


