/* resched.c  -  resched MVS-Xinu dispatcher */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>	// wakeup()
#include <setjmp.h>	// Standard C header

#undef	DEBUG
#include <debug.h>

#define	DEBUG_NL	""	// "\n" or ""
				// use "\n" during debug msg storms
				// for improved messsage identification

#undef	DEBUG_STACK		// stackstate control

/*------------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:
 *	Upon entry, currpid gives current process id.
 *	Proctab[currpid].pstate gives correct NEXT state for
 *	current process if other than PRREADY.
 *
 *	This version of resched is non-preemptive.
 *	We call wakeup(), since MVS-Xinu clkint doesn't
 *	This marks suspended (sleeping) processes ready once their
 *	sleep interval has expired
 *
 *	This version also uses setjmp/longjmp in lieu of procsw
 *	Thanks to Dave Rivers at Dignus for reminding me setjmp existed
 *
 *	WARNING: this code calls procsw(), which insists that fpr[]
 *	immediately follow gpr[] in struct pentry
 *------------------------------------------------------------------------
 */

int resched() {

//  Note R6 is used to prevent the return sequence from the new process
//  (initial dispatch) from referencing R13 before it can be restored
//  Curious parties are directed to the Systems/ASM resched listing

	int		pid;
	int		wakeret;
	void		*poldgpr;
	void		*pnewgpr;
	__register(0) 	void	*r0;			// environment ptr for new process
	__register(1) 	void	*r1;			// standard MVS parm reg
	__register(6)	pentry	*nptr;			// pointer to new process entry
	__register(7)	pentry	*optr;			// pointer to old process entry
	__register(13) 	unsigned int r13;		// standard MVS savearea ptr reg

	__register(15) 	void (* r15)();			// @ initial new proc rtn

//	R15 is a pointer to a function accepting no arguments returning void


	iohook();					// process mgt + I/O mgt intersection

	proclock();
	optr= &proctab[currpid];			// the old process is the one
							// that called us; get the addr
							// of its pentry before we 
							// update currpid

	trace(DEBUG_NL "RESCHED ****** ENTERED; "
		"currpid %d 0x%x process %s pentry 0x%x; preempt %d\n",
		currpid, currpid, optr->pname, optr, preempt);

#ifdef	DEBUG_STACK
	stackstate(NULL, "RESCHED AT ENTRY");		// stack diagnostics
#endif

#ifdef	SCHED_NO_PREEMPT				// wake up sleeping processes
	wakeret = wakeup();				// may mark process(es) READY
	#ifdef	DEBUG
	if (wakeret)					// if wakeup awoke process(es)
		procstate(NPROC, "RESCHED awoke");
	#endif
#endif


//  Determine whether the current process needs to lose control of the cpu
//  No switch needed if all of the following are true:
//	current process hasn't consumed its time interval
//	current process state is still CURRENT
//	current process priority higher than next

	if (
#ifdef	RTCLOCK
	   (preempt > 0) &&
#endif
	   (optr->pstate == PRCURR) &&
	   (lastkey(rdytail) < optr->pprio) ) {
		procunlock();
		return(OK);				// current process retains control of cpu
	}

//  Context switch will occur, mark currently executing process inactive

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid, rdyhead, optr->pprio);
	}

//  Select next process to execute

//  Remove highest priority process at end of ready list preparatory to
//  dispatching it

//  Because the currpid = pid assignment occurs outside control of a lock,
//  no other code in MVS-Xinu is allowed to update currpid, including
//  (indeed, especially) asynchronuously executed code such as code
//  running under the auspices of our STIMER exit

//  We don't mind being interrupted, but we definitely don't want the
//  multiple instructions for the assignment to be corrupted by others'
//  updates to currpid.
//  Note: this is slightly misleading, since we don't perform preemption,
//  but if we ever do, it might prove worth consideration

	lastpid = currpid;
	pid = getlast(rdytail);				// next-to-run process
	procunlock();					// release lock under old-proc

	currpid = pid;					// mark ready process as current

	proclock();					// seize lock under new-proc
	nptr = &proctab[currpid];			// pentry for next-to-run process

//	if (currpid) {					// be quiet about nulluser
		trace("RESCHED NEWPID %d 0x%x process %s pentry 0x%x\n",
			currpid, currpid, nptr->pname, nptr);
//	}

	nptr->pstate = PRCURR;				// mark it currently running;
							// it will be soon

#ifdef	RTCLOCK
	preempt = QUANTUM;				// reset preemption counter
#endif

//  **************************************************************************
//  Dispatch the new current process, either by starting it if it's never been
//  run before, or by restarting it if it has already been started
//  The distinction is necessary so that we can use the Systems/C environment
//  to properly maintain the MVS savearea chain, etc. (where the value of etc
//  is largely unknown except to the compiler vendor ;-) )
//  **************************************************************************

	if ( (nptr->pflag & PEFINIT) == 0) {		// new proc requires initialization?

							// yes: finish initializing process
							// (begun by create),
							// and perform first dispatch
							// for the newly created process

		//  First, save the old process' regs

		poldgpr = &optr->gpr;			// @ current pid's GPR storage
		procsave(poldgpr);			// save current pid's regs

//		trace("RESCHED ** SAVEREGS ** pid %d\n", optr->ppid);
//		snap("RESCHED ** SAVEREGS **", (void *)poldgpr, 16*4);

//		snap("RESCHED old pentry post-procsave", (void *)optr, sizeof(pentry));
//		snap("RESCHED new pentry post-procsave", (void *)nptr, sizeof(pentry));

		nptr->pflag |= PEFINIT;			// new proc now initialized
		procstate(NPROC, "RESCHED pre NEWPROC");
		procunlock();

		trace("RESCHED ****** DISPATCHING NEW PID %d process %s\n", 
			nptr->ppid, nptr->pname);

		//  Here we go ... dispatch the new process for the first time

		//  Initial functions of a process must use the "DCALL=SUPPLIED" 
		//  pragma (see mvs38j.h) and receive control with R0 containing
		//  the environment pointer returned from "DCALL=ALLOCATE"
		//  (which occurs in mvsenv(ENVCREATE,pptr))

		//  We create the environment ourselves rather than letting Systems/C
		//  do so because we may want to reuse the environment in future
		//  versions of this code, since there is some overhead associated
		//  with the creation of an environment

		optr = &proctab[nulldisppid];		// for when we return from new pid

		nptr->reschwk[0] = (void *)&proctab[0];	// save ptr to nulluser pentry
		nptr->reschwk[1] = proctab[0].gpr;	// save ptr to nulluser regs
		nptr->reschwk[2] = proctab[0].gpr[13];	// when old proc = nulluser

		r0 = nptr->penv;			// set env parm ptr reg
		r1 = 0;					// no parms passed
		r15 = (void *)nptr->paddr;		// entry point address
		r13 = (unsigned int)nptr->topsa;	// R13 @ process' top MVS savearea
		(*r15)();				// initiate the newly created process

		//  When control returns to us here, the process is complete
		//  Our problem now is that we need to be able to run under SOME
		//  process' context; the only process we know that always exists 
		//  is nulluser, pid 0, so that's who we pick

		//  The following C code is implemented in assembler, so we
		//  don't have to worry about the vagaries of what the
		//  compiler might generate

		// procload(&optr->gpr);		// restore nulluser regs
		// longjmp(nulljump, (int)nptr);	// nulldisp

		r15 = &optr->gpr;			// procload parm addr

		__asm {
procload ALIAS C'procload'
         EXTRN procload
longjmp  ALIAS C'longjmp'
         EXTRN longjmp
nulljump ALIAS C'nulljump'
         EXTRN nulljump
*
*  We trust that the program base (R12) was correctly restored by the
*  new process that just completed, but other GPRs are for the
*  wrong context (especially R13); reload sufficient regs to 
*  call longjmp, which will reset the regs & jump into nulluser()
*
         ST    R6,LONGPRM2             nptr for longjmp call
         ST    R15,LOADPARM            @ nulluser GPRs & FPRs
         LA    R13,RESCHSA             savearea for procload
         LA    R1,LOADPARM             @ procload parmlist
         L     R15,PROCLD              @ procload()
         BALR  R14,R15                 restore nulluser regs
         STM   R0,R15,LASTREGS         debug: running regs
         LA    R15,LASTREGS            tell C code where regs are
*
*  Set up longjmp parmlist, call it
*
         LA    R1,LONGPRM1             @ longjmp parmlist
         L     R15,VLONGJMP            @ longjmp()
         BALR  R14,R15                 longjmp(nulljump, (int)nptr);
         EX    0,*                     longjmp never returns (S0C3)
*
         DC    0F'0',C'GLOP'
LOADPARM DC    F'-1'                   procload parm: gpr/fpr addr
*
PROCLD   DC    V(procload)             addr of procload()
VLONGJMP DC    V(longjmp)              addr of longjmp()
*
LONGPRM1 DC    V(nulljump)             longjmp parm 1: env
LONGPRM2 DC    F'-1'                   longjmp parm 2: nptr
*
RESCHSA  DC    18F'-1'                 savearea for procload
LASTREGS DC    16F'-1'                 debug: regs from procload
*
         U#EQU                         R0:R15 equates
		}

			//  ****************************************************
	} else {	//  The current process has already been started at some
			//  time in the past, just restore its regs and return
			//  which effectively makes it the current process
			//  ****************************************************

		trace("RESCHED ****** PROCSW; "
			"OLDPID %d 0x%x process %s; NEWPID %d 0x%x process %s\n", 
			optr->ppid, optr->ppid, optr->pname,
			nptr->ppid, nptr->ppid, nptr->pname);

//		stackstate(NULL, "RESCHED pre PROCSW");

		poldgpr = &optr->gpr;			// @ old proc GPRs & FPRs
		pnewgpr = &nptr->gpr;			// @ new proc GPRs & FPRs

		//  Save old process' regs, load new process' regs

		procsw(poldgpr, pnewgpr);		// run new-proc

		trace("RESCHED PROCSW returned, currpid %d\n", currpid);

		procunlock();
	}

//	stackstate(NULL, "RESCHED SAVEAREA CHAIN AT EXIT FROM RESCHED");

	return OK;
}




