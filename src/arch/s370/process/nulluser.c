// nulluser.c - nulluser

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <ctc.h>	// ctcecb

#define	DEBUG
#undef	DEBUG_STATE			// defined = call <component>state()
#define	DEBUG_NL	""		// "\n" or ""
#include <debug.h>

	WORD	nullecb = 0;	// initial value = not waiting, not posted
				// see SYS1.AMODGEN(IHAECB)

/*------------------------------------------------------------------------
 *  nulluser  -- null process (pid==0)
 *------------------------------------------------------------------------
 */
void nulluser()	{			// run when no READY work

        int	userpid, rc;
	pentry	*pptr;

	kprintf(DEBUG_NL "NULLUSER entered, pid %d\n", currpid);

#ifdef	DEBUG_STATE
	stackstate(NULL, "NULLUSER SAVEAREA CHAIN AT ENTRY");
#endif

//  Wait until MVSTMPOP posts our ECB, then call resched to see if
//  there is any work to do; if so, resched runs that work and then
//  returns to us when there is no more ready work

	while (shutxinu == FALSE) {

		resched();				// run other processes

#ifdef	DEBUG_STATE
		procstate(NPROC, "NULLUSER post RESCHED");
		memstate("NULLUSER post RESCHED");
		clockstate(NULL, "NULLUSER post RESCHED");
#endif

		//  When resched returns to us, there is no work to run
		//  WAIT for either nullecb or ctcecb to be POSTed
		//  We clear nullecb, the CTC I/O procedure clears ctcecb

		nullecb = 0;

		mvswaitlist(1, 2, &nullecb, &ctcecb);	// WAIT for work

		if (ctcecb & 0x40000000) {
			trace("\n");
			trace("NULLUSER awakened by CTC\n");
			trace("NULLUSER ecb nullecb %x, addr %x\n", nullecb, &nullecb);
			trace("NULLUSER ecb ctcecb  %x, addr %x\n",  ctcecb, &ctcecb);
		}

	}

} /* nulluser */ 




