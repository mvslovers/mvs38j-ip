/* xdone.c - xdone */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <xgd.h>
#include <clock.h>

/*------------------------------------------------------------------------
 * xdone  --  print system completion message as last process exits
 *------------------------------------------------------------------------
 */
void xdone() {

	__register(8) 	int r8;		// XGD length
	__register(9) 	void *r9;	// XGD address
	sgd	*psgd;

	kprintf("All MVS-Xinu user processes completed\n");

	mvsclkoff();			// cancel remaining STIMER

#ifdef RET_WHEN_DONE    
	
	/* loop in case we <C>ontinue from the monitor */
	while (TRUE) {
		ret_mon();
		kprintf("\n\nNo Xinu user processes remaining.\n\n");
	}
#endif

#ifdef	MVS_XDONE_DEBUG
	panic(MVS_XDONE_DEBUG);		// produce component summary
#endif

	mvsfreemem();			// release MVS memory

//  Release storage for XGD that was allocated in sysinit()

	psgd = findsgd();
	r8 = sizeof(xgd);
	r9 = psgd->sgd_xgd;
	s370state(SUPERVISOR);		// assure supvr state
	mvskey0();			// enter SYSTEM key
	psgd->sgd_xgd = 0;		// zero SGD ptr to XGD
	__asm {
         FREEMAIN RU,A=(9),LV=(8),SP=241      free XGD
	}
	mvskey8();			// back to USER key

//  Issue SYSEVENT OKSWAP
//  MVS-Xinu startup (sysinit) issued SYSEVENT DONTSWAP

	mvsevent(OKSWAP);		// we're swappable again

	s370state(PROBLEM);		// return to MVS in problem state

//	snap("XDONE sgd", psgd, sizeof(sgd));

	exit(0);			// bye bye
}




