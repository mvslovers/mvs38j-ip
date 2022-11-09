/* procstate.c - Debug Aid - display process table info */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

#define	DEBUG_NL	"\n"	// "\n" or ""

/*------------------------------------------------------------------------
 * pentry_state  --  display process table info for one process
 *------------------------------------------------------------------------
 */
void pentry_state(int i) {

	pentry	*pent;
	void	*pvoid;
	Bool	show;
	char	bufcall[80];

	pent = &proctab[i];

	kprintf("<PENTRY> 0x%x pid %d pri %d %s %s\n"
//		"         pflag %x prio %d sem %d kin %d\n"
//		"         penv 0x%x topsa 0x%x\n"
		,
		pent,
		i,
		pent->pprio,
		procstatdesc[ pent->pstate - 1 ], 
		pent->pname
//		pent->pflag,
//		pent->pprio,
//		pent->psem,
//		pent->pnxtkin,
//		pent->penv,
//		pent->topsa
		);
	switch (pent->pstate) {
		case PRWAIT: 	show = TRUE; break;
		case PRREADY: 	show = TRUE; break;
		case PRRECV: 	show = TRUE; break;
		case PRCURR: 	show = FALSE; break;	// ABENDS0C4; haven't looked at it
		default:	show = FALSE;
	}
	if (show) {
		pvoid = (void *)pent->gpr[13];			// start with proc's current R13
		funchist(bufcall, sizeof(bufcall), pvoid);	// function call history
		kprintf("         %s\n", bufcall);
	}

#ifdef	MVS_DUMP_PROCSTATE
	snap("PENTRY", (void *) &proctab[i], sizeof(struct pentry));
#endif

	return;
}

/*------------------------------------------------------------------------
 * procstate  --  display process table info or just one process
 * 	When pid = NPROC, whole proces table is displayed (except PRFREEs)
 *	When msg = NULL, caller's function name is used for the message
 *------------------------------------------------------------------------
 */
void procstate(int pid, char *msg) {

	int	i;
	char	bufcaller[FUNCSTR_BUFLEN];

	if (msg == NULL) {
		funcstr(bufcaller, MY_CALLER);
		msg = bufcaller;
	}

	kprintf(DEBUG_NL "PROCSTATE currpid %d, preempt %d %s\n", 
		currpid, preempt, msg);

	if (pid != NPROC) {
		pentry_state(pid);	// just do one
		return;
	}

	for (i=0; i<NPROC; i++) {
		if (proctab[i].pstate == PRFREE) continue;
		pentry_state(i);
	}
//	kprintf("PROCSTATE exit, currpid %d\n\n", currpid);
	return;
}

