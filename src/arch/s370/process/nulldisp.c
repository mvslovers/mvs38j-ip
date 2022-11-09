// nulldisp.c - nulldisp

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <setjmp.h>	// Standard C header

#define	DEBUG
#undef	DEBUG_STATE			// defined = call <component>state()
#define	DEBUG_NL	""		// "\n" or ""
#include <debug.h>

	jmp_buf	nulljump;		// setjmp/longjmp environment

/*------------------------------------------------------------------------
 *  nulldisp	nulluser helper process, interface to process management
 *		
 *------------------------------------------------------------------------
 */

PROCESS	nulldisp() {

        int	jmpret;

//  Use setjmp to establish an environment in which resched can longjmp
//  to us when a process completes

//  Our sole reason for existing is that nulluser's DSA environment
//  isn't reliable enough for resched to longjmp to nulluser

//  We want to be entirely out of the completed process before we start
//  tearing it down

	nulldisppid = currpid;				// tell resched our pid
	while (shutxinu == FALSE) {

		if ( (jmpret = setjmp(nulljump)) == 0) {

			;				// code path for nulluser mainline

		} else {				// code path following longjmp in resched

							// terminate completed process
			nullkill((pentry *)jmpret, nulldisppid);
		}

		suspend(nulldisppid);			// go to sleep until we're needed
		
	}
}

 

