/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */

SYSCALL	receive() {

	struct	pentry	*pptr;
	WORD	msg;


	trace("RECEIVE enter; currpid %d\n", currpid);

	memlock();
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		trace("RECEIVE pid %d %s state PRRECV; waiting for message\n", 
			currpid, pptr->pname);
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	memunlock();

	trace("RECEIVE exit; currpid %d %s; returning received message 0x%x\n", 
		currpid, pptr->pname, msg);
	return(msg);
}


