/* mvsunlock.c - MVS-specific lock management support */

#ifdef	MVS_LOCKMGR

#include <conf.h>
#include <kernel.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * mvsunlock - release control of lock
 * If lock is not available, suspend process requesting lock
 *------------------------------------------------------------------------
 */
void mvsunlock(int which, char *who) {
	__register(6)   void * r6;	// lock value
	__register(7)   void * r7;	// @ pentry.lock
	__register(8)   void * r8;	// pentry.lock when busy
	__register(15)   void * r15;	// debug aid
	int		ownpid;		// pid owning k-lock
	int		nlocks;

#ifdef MVS_TRACE_LOCK
	kprintf("MVSUNLOCK entry; caller %s pid %d, lock %d owner = %d 0x%x\n", 
		who, currpid, which, locks[which].owner, locks[which].owner);
#endif
	if ( (which < 0) || (which > MAXLOCK)) {
		panic("mvsunlock: lock number");
	}

//  Lock the process's pentry control block so we can safely update nlocks.

	r7 = &(proctab[currpid].lock);	// @ pentry lockword
	__asm {
         ICM   15,15,=C'LOCK'     debug aid
         BALR  6,0                convenient lock value
         ULOCK 6,0(7),OLD=8,BUSY=NOTAVAIL
         SR    6,6                indicate pentry.lock held
NOTAVAIL DS    0H
	}
	if (r6 != 0)
		mvsabend("mvsunlock pentry.lock busy", 102);

//  If we didn't branch to ABEND, we now hold pentry.lock,
//  meaning we own the process entry.

//  Verify pid attempting release owns lock

	ownpid = locks[which].owner;
	if (ownpid == LOCKNULL) 
		ownpid = 0;			// adjust nulluser pid
	if (currpid != ownpid) {
		snap("MVSUNLOCK locks", (void *) &locks, sizeof(locks));
		panic("mvsunlock: pid != owner");
	}

//  Only release the lock when its count goes to zero

	locks[which].count--;
	if (locks[which].count == 0) {
		locks[which].owner = 0;		// release lock
		proctab[currpid].nlocks--;	// pid owns one less lock
	}

//  Release pentry.lock, issue trace msg & exit

	nlocks = proctab[currpid].nlocks;
	proctab[currpid].lock = 0;	// release pentry lock
#ifdef MVS_TRACE_LOCK
	kprintf("MVSUNLOCK exit, pid %d proc %s nlocks %d\n", 
		currpid, proctab[currpid].pname, nlocks);
#endif
	return;

	__asm {
         LTORG ,
	}

} /* mvsunlock */

#endif	/* MVS_LOCKMGR */

