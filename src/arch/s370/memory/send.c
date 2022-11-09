/* send.c - send */

//  MVS-Xinu	Systems/C has a send routine; renamed to xsend

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <clock.h>

#define DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  send  --  send a message to another process
 *------------------------------------------------------------------------
 */

SYSCALL xsend(int pid, WORD msg) {

	struct	pentry	*pptr;


	trace("SEND enter; currpid %d %s; dest pid %d %s; msg 0x%x\n", 
		currpid, proctab[currpid].pname, pid, proctab[pid].pname, msg);
	memlock();
	pptr = &proctab[pid];
	if (isbadpid(pid) 
	   || (pptr->pstate == PRFREE)
	   || pptr->phasmsg != 0) {
		memunlock();
		callinfo("SEND");
		trace("SEND syserr; pid %d pentry 0x%x phasmsg %x\n",
			pid, pptr, pptr->phasmsg);
		snap("SEND pentry", (void *)pptr, sizeof(struct pentry));
		return(SYSERR);
	}
	pptr->pmsg = msg;
	pptr->phasmsg = TRUE;
	if (pptr->pstate == PRRECV) {	// if receiver waiting, start it
		trace("SEND ready pid %d %s (reschyes)\n", pid, proctab[pid].pname);
		ready(pid, RESCHYES);
	} else if (pptr->pstate == PRTRECV) {
		trace("SEND awaken pid %d %s (reschyes)\n", pid, proctab[pid].pname);
		unsleep(pid);
		ready(pid, RESCHYES);
	}
	memunlock();
	trace("SEND exit; OK\n");
	return(OK);
}






