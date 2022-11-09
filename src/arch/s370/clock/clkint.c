/* clkint.c - clkint */

// TODO review p 158 clkint, and i386/asm/clkint.asm
// TODO this code doesn't currently preempt

//  This code is called by clkinit() as its own process.
//  We are responsible for saving our regs in our pentry, such
//  that MVSTMPOP can restore them to resume our execution
//  as indicated below

#include <conf.h>
#include <kernel.h>
#include <clock.h>
#include <setjmp.h>

jmp_buf	clkenv;

void clkint_test() {
	return;
}

/*
 *------------------------------------------------------------------------
 * clkint - timer pop exit called by MVSTMPOP in mvsclkinit.asm
 *------------------------------------------------------------------------
 */
void clkint() {
	pentry		*pptr;			// @ my pentry
	WORD		*pgpr;			// @ pentry GPRs
	int		jmpret;			// setjmp return value
	__register(6)	void *r6;
	__register(7)	void *r7;

// ****************************************************************************
//  Enter here from clkinit()
// ****************************************************************************

	kprintf("CLKINT ****** entered from SYSINIT ******\n");
	pptr = &(proctab[currpid]);		// my pentry ptr

#if 0
	pgpr = (WORD *) &(pptr->gpr);		// @ current pid's GPR storage
	pclkint = (void *) pgpr;		// @ regs in pentry
	snap("CLKINT pentry pre procsave", pptr, sizeof(pentry));
	procsave(pgpr);				// save current regs
	snap("CLKINT pentry post procsave", pptr, sizeof(pentry));
#endif

//  We need to give the compiler a clue that we're doing something weird.
//  setjmp is just the ticket.
//  It's OK for us to return to MVSTMPOP with our registers trashed

	if ((jmpret = setjmp(clkenv)) == 0) {
		__asm {
         LA    R6,CLKRESUM        tell MVSTMPOP where to restart us
         STM   R0,R15,CLKSAVE     save regs for CLKRESUM
         LA    R6,CLKSAVE         set R6 for MVSTMPOP
		}
		snap("CLKINT CLKENV", &clkenv, 18 * 4);
		snap("CLKINT CLKRESUM regs established", r6, 16 * 4);
		pclkint = r6;		// tell MVSTMPOP where to find CLKRESUM regs
		pptr->pstate = PRSUSP;
		kprintf("CLKINT pptr 0x%x, pgpr 0x%x, pclkint 0x%x\n", pptr, pgpr, pclkint);
		kprintf("CLKINT ****** SUSPENDING\n");
		resched();		// process never awakened
		mvsabend("clkint resumed erroneously", 69);
	}

//  Return from longjmp resumes here:

	__asm {
         BALR  R14,0
         DC    H'0'
	}
//		kprintf("CLKINT longjmp ******\n");
	__asm {
         L     R14,CLKENTRY+(14*4)          MVSTMPOP return address
         BR    R14                          return to MVSTMPOP
	}
// ****************************************************************************
//  Enter here from MVSTMPOP
// ****************************************************************************
//  The following code must not issue I/O; MVS and/or Systems/C doesn't seem
//  to take a favorable view of issuing I/O (or at least kprintf, which is what
//  I observed to cause problems) from a STIMER exit.  WTO doesn't seem to 
//  cause problems.

	__asm {
         ENTRY CLKRESUM
         DS    0D
         DC    A(0)
ADDRENV  DC    A(clkenv)
**
         DC    CL8'CLKSAVE'
CLKSAVE  DC    16F'0'             clkint.CLKRESUM regs for MVSTMPOP
**
         DC    CL8'CLKENTRY'
CLKENTRY DC    16F'0'             clkint.CLKRESUM regs @ entry
         DC    CL8'CLKEYE'
**
         DS    0D                 16 byte CLKRESUM prefix
         DC    CL8'CLKRESUM'
         DC    A(CLKRESUM)                  CLKRESUM code
         DC    A(CLKSAVE)                   CLKRESUM regs R0:R15
**
CLKRESUM DS    0D
         STM   R0,R15,CLKENTRY              debug aid
         CLC   CLKSAVE(14*4),CLKENTRY       assert R0:R13 as requested
         BE    CLKGOOD
**
         BALR  R14,0                        debug aid
         DC    H'0'
         WTO   'CLKRESUM reg fail'
         ABEND 69,DUMP
**
CLKGOOD  LA    R6,CLKENTRY
         WTO   'CLKRESUM ****** ENTERED'
	}

	clkint_test();

	__asm {
         L     R14,CLKENTRY+(14*4)          MVSTMPOP return address
         BR    R14                          return to MVSTMPOP
         BALR  R14,0
         DC    H'0'
	}

//	longjmp(clkenv, 1);	// restore environment, branch back to mainline

	__asm {
         PUSH  PRINT
         PRINT NOGEN
         U#EQU ,                                 GPR equates
         POP   PRINT
	}
} 

