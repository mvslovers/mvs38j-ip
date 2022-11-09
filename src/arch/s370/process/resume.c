/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

#define	DEBUG
#include <debug.h>

#define	DEBUG_NL	""	// "\n" or ""

/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */

SYSCALL resume(int pid) 
{
	STATWORD 	ps;    
	struct		pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int		prio;			/* priority to return		*/
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(funcstr(bufcall, MY_CALLER));
	trace(DEBUG_NL "RESUME enter; resume pid %d, currpid %d <<%s>>\n", 
		pid, currpid, bufcall);

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
		trace("RESUME syserr\n");
		return(SYSERR);
	}

	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);

	trace("RESUME exit; readied pid %d, currpid %d\n \n", pid, currpid);
	return(prio);
}


