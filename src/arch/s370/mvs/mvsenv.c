/* mvsenv.c - MVS-specific Systems/C-specific environment support */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>

#undef	DEBUG					// defined = display debug info
#undef	DEBUG_ABEND_ENVCREATE			// defined = abend before ENVCREATE exit
#include <debug.h>

// Stub routines to support DCALL= requests

void *envcreate() { }
void envbegin() { stackstate(NULL, "ENVBEGIN ENVBEGIN ENVBEGIN"); }
void envdestroy() { }

/*----------------------------------------------------------------------------
 * mvsenv - Systems/C environment support
 *
 *	req		ENVCREATE, ENVBEGIN, or ENVDESTROY
 *	ppentry		addr of related struct pentry
 *----------------------------------------------------------------------------
 */

void *mvsenv(int req, pentry *ppentry) {
	__register(0)   void * r0;
	__register(1)   void * r1;
	__register(8)   void * r8;

	switch (req) {

// *----------------------------------------------------------------------
// *  Create Systems/C environment
// *----------------------------------------------------------------------
	case ENVCREATE:

#ifdef	DEBUG
		trace("MVSENV envcreate enter\n");
		__asm {
         STM   R0,R15,REGSAVE
         LA    R8,REGSAVE
		}
		snap("MVSENV ENVCREATE regs prior to envcreate()", r8, 16*4);
#endif

		envcreate();		// create new Systems/C environment

#ifdef	DEBUG
		__asm {
         STM   R0,R15,REGSAVE
         LA    R8,REGSAVE
		}
		snap("MVSENV ENVCREATE regs after envcreate()", r8, 16*4);
		__asm {
         L     R1,REGSAVE+4       restore R1 from envcreate() return
		}
#endif

		r8 = r1;		// save env ptr in safer reg
		ppentry->penv = r8;
		ppentry->pflag |= PEFCREATE;

#ifdef	DEBUG
		snap("MVSENV post-ENVCREATE pentry", (void *)ppentry, sizeof(pentry));
		stackstate(NULL, "MVSENV before exit");
		trace("MVSENV envcreate exit\n");
#endif

#ifdef	DEBUG_ABEND_ENVCREATE
		__asm {  DC H'0' }	// sleazy, but useful
#endif

		return r8;		// return Systems/C environment ptr

// *----------------------------------------------------------------------
// *  Begin using previously created Systems/C environment
// *----------------------------------------------------------------------
	case ENVBEGIN:
		trace("MVSENV envbegin enter\n");
		snap("MVSENV pentry", ppentry, sizeof(pentry));
		if (ppentry->penv == NULL) {
			panic("mvsenv ENVBEGIN");
		}
		ppentry->pflag |= PEFINIT;
		trace("MVSENV envbegin calling envbegin\n");
		r0 = ppentry->penv;	// set env parm ptr reg
		envbegin();
		r8 = &ppentry->gpr;
		__asm {
         STM   0,15,0(8)               save initial process regs
		}
		snap("MVSENV pentry", ppentry, sizeof(pentry));
		trace("MVSENV envbegin exit\n");
		return NULL;


// *----------------------------------------------------------------------
// *  Destroy Systems/C environment
// *----------------------------------------------------------------------
	case ENVDESTROY:
		trace("MVSENV envdestroy enter\n");
		snap("MVSENV pentry", ppentry, sizeof(pentry));
		r0 = ppentry->penv;	// set env parm ptr reg
		envdestroy();
		ppentry->penv = NULL;	// environment gone
		ppentry->pflag = 0;
		trace("MVSENV envdestroy exit\n");
		return NULL;


	default:
		kprintf("MVSENV unrecognized request %d ignored\n", req);
		break;
	}
	return NULL;

#ifdef	DEBUG
	__asm {
REGSAVE  DC    16F'-1'
         PUSH  PRINT
         PRINT NOGEN
         U#EQU ,                  register equates
         POP   PRINT
	}
#endif

} /* mvsenv */


