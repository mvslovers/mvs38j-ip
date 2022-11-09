/* intr.c - S/370 interrupt support */

// --------------------------------------------------------------------------------------
//  This code contains inline assembler code as supported by the Dignus Systems/C
//  compiler.  See src/arch/s370/doc/SystemsC-notes.txt for some further details.
// --------------------------------------------------------------------------------------

#include <conf.h>
#include <kernel.h>

#define	GEN_ABLE_ASM	0	// 1 = generate enable(), disable(), restore() asm code
#define	TRACE_ABLE	0	// 1 = generate kprintf() stating enable(), etc callers

#ifdef	MVS38J

#ifdef	MVS_TSO_TEST
// --------------------------------------------------------------------------------------
//  mvsauth() - enable (req=1), disable (req=0) MVS APF authorization
//		allows TSO TEST usage with MVS-Xinu, requires magic SVC 245
//		WARNING: TSO TEST SVC 97 currently has problems, code should be disabled
// --------------------------------------------------------------------------------------

int mvsauth(int req) {
	__register(6)	int	r6 = MVS_MAGIC_SVC;

	if (req) {
		__asm {
	L      0,=C'AUTH'
        EX     6,MAGICSVC                        JSCBAUTH=1
        LR     6,15                              return SVC rc
		}
	} else {
		__asm {
	SLR    0,0
        EX     6,MAGICSVC                        JSCBAUTH=0
        LR     6,15                              return SVC rc
		}
	}
	kprintf("MVSAUTH %s APF rc %d\n", (req) ? "enable" : "disable", r6);
	return r6;

	__asm {
MAGICSVC SVC   0
         LTORG ,
	}
}
#endif /* MVS_TSO_TEST */

// --------------------------------------------------------------------------------------
//  testauth() - test APF authorization
//  returns: 0 - APF authorized, else not APF authorized
// --------------------------------------------------------------------------------------

int testauth() {
	__register(15) unsigned long r15;	 	// TESTAUTH sets R15

	__asm {
	TESTAUTH FCTN=1	
	}
	return r15;					// R15=0 means APF authorized
} 

// --------------------------------------------------------------------------------------
//  suprstate() - place MVS-Xinu in S/370 supervisor state
//  MVS-Xinu never leaves supervisor state
// --------------------------------------------------------------------------------------

void mvsstate(int mode, char *who) {	// mode 0 = supervisor, mode 1 = problem
	int	rc;

#ifdef	MVS_TSO_TEST
	rc = testauth();		// rc=0 means APF auth'd
	if (rc) {
		rc = mvsauth(1);	// enable APF authorization
		kprintf("S370STATE mvsauth rc %d\n", rc);
	}
#endif

	if (mode == SUPERVISOR) {
		kprintf("S370STATE supervisor, %s\n", who);
		__asm {
	MODESET MODE=SUP	enter S/370 supervisor state
		}
	} else {
		kprintf("S370STATE problem, %s\n", who);
		__asm {
	MODESET MODE=PROB	enter S/370 problem state
		}
	}
	return;
} 

// --------------------------------------------------------------------------------------
//  mvsdisable: disable interrupts, save mask in ps; S/370 EC PSW assumed
// --------------------------------------------------------------------------------------

void mvsdisable(STATWORD ps, char *who) {
	__register(5) unsigned long r5 = &ps;	 	// location to save old sys mask
#if TRACE_ABLE
	kprintf("DISABLE %s\n", who);
#endif
#if GEN_ABLE_ASM
	__asm {
         STNSM 0(5),B'11111101'       disable I/O, leave PER & EXTERNAL alone
*                      R   TIE        Restart, daT, I/o, External
	}
#endif
	return;
} 

// --------------------------------------------------------------------------------------
//  mvsenable: save current system mask, enable interrupts; S/370 EC PSW assumed
// --------------------------------------------------------------------------------------
void mvsenable(char *who) {
	int	dummy;
	__register(5) unsigned long r5 = &dummy;	// location to save old sys mask
#if TRACE_ABLE
	kprintf("ENABLEd %s\n", who);
#endif
#if GEN_ABLE_ASM
	__asm {
         STOSM 0(5),B'00000010'       enable I/O interrupts
	}
#endif
	return;
}

// --------------------------------------------------------------------------------------
//  mvsrestore: restore system mask
// --------------------------------------------------------------------------------------

void mvsrestore(STATWORD ps, char *who) {
	int		work;
	__register(5) unsigned long r5 = &ps;		// location to save old sys mask
	__register(6) unsigned long r6 = &work;		// temp save
#if TRACE_ABLE
	kprintf("RESTORE %s\n", who);
#endif
#if GEN_ABLE_ASM
	__asm {
         STNSM 0(6),B'11111101'       disable all interrupts
         STOSM 0(5),B'00000010'       enable I/O interrupts
	}
#endif
	return;
}
#endif	/* MVS38J */

// --------------------------------------------------------------------------------------
//  halt: infinite loop
// --------------------------------------------------------------------------------------
//void halt() {
//	pause();
//}

// --------------------------------------------------------------------------------------
//  pause: enable interrupts, enter spin loop
// --------------------------------------------------------------------------------------
void pause() {
	enable();			/* enable interrupts */
	kprintf("PAUSE\n");
//	halt();				/* bogus spin until interrupt */
	return;				/* stub */
}


